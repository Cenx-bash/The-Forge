package com.firewall.expert;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import java.time.*;
import java.nio.file.*;
import java.util.logging.*;
import java.security.*;
import java.util.regex.*;

public class AdvancedNetworkFirewall {
    private static final Logger logger = Logger.getLogger(AdvancedNetworkFirewall.class.getName());
    
    private final ExecutorService threadPool;
    private final Map<String, FirewallRule> rules;
    private final RateLimiter rateLimiter;
    private final ThreatIntelligence threatIntel;
    private final ConnectionTracker connectionTracker;
    private final MetricsCollector metrics;
    private final Configuration config;
    private ServerSocket serverSocket;
    private volatile boolean running;
    
    // Rule types
    public enum RuleType {
        ALLOW, DENY, RATE_LIMIT, LOG_ONLY, QUARANTINE
    }
    
    public enum Protocol {
        TCP, UDP, ICMP, ANY
    }
    
    public enum Action {
        ALLOW, DROP, REJECT, LOG, ALERT, QUARANTINE
    }
    
    // Firewall Rule definition
    static class FirewallRule {
        String ruleId;
        String name;
        RuleType type;
        Protocol protocol;
        String sourceIp;
        String destIp;
        Integer sourcePort;
        Integer destPort;
        String application;
        Instant validFrom;
        Instant validUntil;
        int priority;
        Map<String, String> metadata;
        boolean enabled;
        
        public boolean matches(Packet packet) {
            // Complex matching logic
            if (!enabled) return false;
            
            Instant now = Instant.now();
            if (validFrom != null && now.isBefore(validFrom)) return false;
            if (validUntil != null && now.isAfter(validUntil)) return false;
            
            boolean protocolMatch = protocol == Protocol.ANY || 
                                   protocol.name().equals(packet.protocol);
            boolean sourceIpMatch = matchesCIDR(packet.sourceIp, sourceIp);
            boolean destIpMatch = matchesCIDR(packet.destIp, destIp);
            boolean sourcePortMatch = sourcePort == null || 
                                     sourcePort == packet.sourcePort;
            boolean destPortMatch = destPort == null || 
                                   destPort == packet.destPort;
            
            return protocolMatch && sourceIpMatch && destIpMatch && 
                   sourcePortMatch && destPortMatch;
        }
        
        private boolean matchesCIDR(String ip, String cidr) {
            if (cidr == null || cidr.equals("ANY")) return true;
            
            try {
                if (!cidr.contains("/")) {
                    return ip.equals(cidr);
                }
                
                String[] parts = cidr.split("/");
                String network = parts[0];
                int prefix;
                
                if (parts[1].contains(".")) {
                    // Subnet mask format
                    prefix = maskToPrefix(parts[1]);
                } else {
                    prefix = Integer.parseInt(parts[1]);
                }
                
                return isInRange(ip, network, prefix);
            } catch (Exception e) {
                return false;
            }
        }
        
        private int maskToPrefix(String mask) {
            String[] octets = mask.split("\\.");
            int prefix = 0;
            for (String octet : octets) {
                int value = Integer.parseInt(octet);
                prefix += Integer.bitCount(value);
            }
            return prefix;
        }
        
        private boolean isInRange(String ip, String network, int prefix) {
            try {
                byte[] ipBytes = InetAddress.getByName(ip).getAddress();
                byte[] netBytes = InetAddress.getByName(network).getAddress();
                
                // Compare bytes based on prefix
                int fullBytes = prefix / 8;
                int remainingBits = prefix % 8;
                
                for (int i = 0; i < fullBytes; i++) {
                    if (ipBytes[i] != netBytes[i]) return false;
                }
                
                if (remainingBits > 0) {
                    int mask = 0xFF << (8 - remainingBits);
                    return (ipBytes[fullBytes] & mask) == (netBytes[fullBytes] & mask);
                }
                
                return true;
            } catch (UnknownHostException e) {
                return false;
            }
        }
    }
    
    // Packet representation
    static class Packet {
        String sourceIp;
        String destIp;
        int sourcePort;
        int destPort;
        String protocol;
        byte[] payload;
        Instant timestamp;
        int size;
        String interfaceName;
        
        @Override
        public String toString() {
            return String.format("%s:%d -> %s:%d [%s] %d bytes", 
                sourceIp, sourcePort, destIp, destPort, protocol, size);
        }
    }
    
    // Connection state tracking
    static class ConnectionState {
        String connectionId;
        String sourceIp;
        String destIp;
        int sourcePort;
        int destPort;
        Protocol protocol;
        Instant startTime;
        Instant lastActivity;
        long bytesSent;
        long bytesReceived;
        State state;
        
        enum State {
            SYN_SENT, ESTABLISHED, FIN_WAIT, CLOSED, RESET
        }
    }
    
    // Rate limiter for DDoS protection
    static class RateLimiter {
        private final Map<String, RateLimitBucket> buckets;
        private final ScheduledExecutorService cleaner;
        
        static class RateLimitBucket {
            String key;
            int limit;
            Duration window;
            Deque<Instant> requests;
            
            synchronized boolean allow() {
                Instant now = Instant.now();
                Instant cutoff = now.minus(window);
                
                // Remove old requests
                while (!requests.isEmpty() && requests.peekFirst().isBefore(cutoff)) {
                    requests.pollFirst();
                }
                
                if (requests.size() < limit) {
                    requests.addLast(now);
                    return true;
                }
                return false;
            }
        }
        
        public RateLimiter() {
            this.buckets = new ConcurrentHashMap<>();
            this.cleaner = Executors.newScheduledThreadPool(1);
            this.cleaner.scheduleAtFixedRate(this::cleanup, 1, 1, TimeUnit.HOURS);
        }
        
        public boolean allow(String key, int limit, Duration window) {
            RateLimitBucket bucket = buckets.computeIfAbsent(key, 
                k -> new RateLimitBucket(limit, window));
            return bucket.allow();
        }
        
        private void cleanup() {
            Instant cutoff = Instant.now().minus(Duration.ofHours(24));
            buckets.entrySet().removeIf(entry -> 
                entry.getValue().requests.peekFirst() != null &&
                entry.getValue().requests.peekFirst().isBefore(cutoff));
        }
    }
    
    // Threat intelligence integration
    static class ThreatIntelligence {
        private final Set<String> maliciousIps;
        private final Set<String> maliciousDomains;
        private final Map<String, String> geoIpDatabase;
        private final ScheduledExecutorService updater;
        
        public ThreatIntelligence() {
            this.maliciousIps = ConcurrentHashMap.newKeySet();
            this.maliciousDomains = ConcurrentHashMap.newKeySet();
            this.geoIpDatabase = new ConcurrentHashMap<>();
            this.updater = Executors.newScheduledThreadPool(1);
            
            // Schedule updates
            this.updater.scheduleAtFixedRate(this::updateThreatFeeds, 
                0, 1, TimeUnit.HOURS);
        }
        
        public boolean isMaliciousIp(String ip) {
            return maliciousIps.contains(ip);
        }
        
        public boolean isMaliciousDomain(String domain) {
            return maliciousDomains.contains(domain.toLowerCase());
        }
        
        public String getGeoLocation(String ip) {
            return geoIpDatabase.getOrDefault(ip, "Unknown");
        }
        
        private void updateThreatFeeds() {
            try {
                // Fetch from external threat feeds
                updateFromAbuseIPDB();
                updateFromFireHOL();
                updateFromGeoIP();
            } catch (Exception e) {
                logger.log(Level.WARNING, "Failed to update threat feeds", e);
            }
        }
        
        private void updateFromAbuseIPDB() {
            // Implement API call to AbuseIPDB
        }
        
        private void updateFromFireHOL() {
            // Implement download from FireHOL IP lists
        }
        
        private void updateFromGeoIP() {
            // Implement GeoIP database update
        }
    }
    
    // Connection state tracker
    static class ConnectionTracker {
        private final Map<String, ConnectionState> connections;
        private final Map<String, Integer> connectionCounts;
        
        public ConnectionTracker() {
            this.connections = new ConcurrentHashMap<>();
            this.connectionCounts = new ConcurrentHashMap<>();
        }
        
        public void trackConnection(Packet packet) {
            String key = generateConnectionKey(packet);
            ConnectionState state = connections.computeIfAbsent(key, k -> 
                createNewConnectionState(packet));
            
            state.lastActivity = Instant.now();
            
            // Update counters
            connectionCounts.merge(packet.sourceIp, 1, Integer::sum);
            
            // Clean old connections
            cleanupOldConnections();
        }
        
        public int getConnectionCount(String ip) {
            return connectionCounts.getOrDefault(ip, 0);
        }
        
        private void cleanupOldConnections() {
            Instant cutoff = Instant.now().minus(Duration.ofMinutes(10));
            connections.entrySet().removeIf(entry -> 
                entry.getValue().lastActivity.isBefore(cutoff));
        }
        
        private String generateConnectionKey(Packet packet) {
            return String.format("%s:%d-%s:%d-%s", 
                packet.sourceIp, packet.sourcePort,
                packet.destIp, packet.destPort,
                packet.protocol);
        }
        
        private ConnectionState createNewConnectionState(Packet packet) {
            ConnectionState state = new ConnectionState();
            state.connectionId = UUID.randomUUID().toString();
            state.sourceIp = packet.sourceIp;
            state.destIp = packet.destIp;
            state.sourcePort = packet.sourcePort;
            state.destPort = packet.destPort;
            state.protocol = Protocol.valueOf(packet.protocol);
            state.startTime = Instant.now();
            state.lastActivity = Instant.now();
            state.state = ConnectionState.State.SYN_SENT;
            return state;
        }
    }
    
    // Metrics and analytics
    static class MetricsCollector {
        private final Map<String, AtomicLong> counters;
        private final Queue<Event> events;
        private final ScheduledExecutorService reporter;
        
        static class Event {
            String type;
            Map<String, Object> data;
            Instant timestamp;
            
            public Event(String type, Map<String, Object> data) {
                this.type = type;
                this.data = data;
                this.timestamp = Instant.now();
            }
        }
        
        public MetricsCollector() {
            this.counters = new ConcurrentHashMap<>();
            this.events = new ConcurrentLinkedQueue<>();
            this.reporter = Executors.newScheduledThreadPool(1);
            
            // Schedule reporting
            this.reporter.scheduleAtFixedRate(this::reportMetrics, 
                1, 1, TimeUnit.MINUTES);
        }
        
        public void incrementCounter(String name) {
            counters.computeIfAbsent(name, k -> new AtomicLong()).incrementAndGet();
        }
        
        public void addEvent(String type, Map<String, Object> data) {
            events.offer(new Event(type, data));
        }
        
        public Map<String, Long> getCounters() {
            Map<String, Long> result = new HashMap<>();
            counters.forEach((k, v) -> result.put(k, v.get()));
            return result;
        }
        
        private void reportMetrics() {
            // Send metrics to monitoring system (Prometheus, Graphite, etc.)
            Map<String, Long> currentCounters = getCounters();
            
            // Export via JMX
            exportJMXMetrics(currentCounters);
            
            // Log summary
            logger.info("Firewall metrics: " + currentCounters);
        }
        
        private void exportJMXMetrics(Map<String, Long> metrics) {
            // Implement JMX MBean registration and updates
        }
    }
    
    // Configuration management
    static class Configuration {
        private final Properties props;
        private final Path configFile;
        private final WatchService watchService;
        private final Thread watcherThread;
        
        public Configuration(String configPath) throws IOException {
            this.configFile = Paths.get(configPath);
            this.props = new Properties();
            loadConfiguration();
            
            // Set up file watcher for hot reload
            this.watchService = FileSystems.getDefault().newWatchService();
            this.configFile.getParent().register(watchService, 
                StandardWatchEventKinds.ENTRY_MODIFY);
            
            this.watcherThread = new Thread(this::watchConfigChanges);
            this.watcherThread.setDaemon(true);
            this.watcherThread.start();
        }
        
        private void loadConfiguration() {
            try (InputStream is = Files.newInputStream(configFile)) {
                props.load(is);
            } catch (IOException e) {
                logger.log(Level.SEVERE, "Failed to load configuration", e);
            }
        }
        
        private void watchConfigChanges() {
            while (true) {
                try {
                    WatchKey key = watchService.take();
                    for (WatchEvent<?> event : key.pollEvents()) {
                        if (event.context().toString().equals(configFile.getFileName().toString())) {
                            logger.info("Configuration file changed, reloading...");
                            loadConfiguration();
                        }
                    }
                    key.reset();
                } catch (Exception e) {
                    logger.log(Level.WARNING, "Config watcher error", e);
                }
            }
        }
        
        public String getString(String key, String defaultValue) {
            return props.getProperty(key, defaultValue);
        }
        
        public int getInt(String key, int defaultValue) {
            try {
                return Integer.parseInt(props.getProperty(key));
            } catch (NumberFormatException e) {
                return defaultValue;
            }
        }
        
        public boolean getBoolean(String key, boolean defaultValue) {
            String value = props.getProperty(key);
            if (value == null) return defaultValue;
            return Boolean.parseBoolean(value);
        }
    }
    
    // Main firewall class
    public AdvancedNetworkFirewall(String configPath) throws IOException {
        this.config = new Configuration(configPath);
        this.threadPool = Executors.newFixedThreadPool(
            config.getInt("firewall.threads", 10));
        this.rules = loadRules();
        this.rateLimiter = new RateLimiter();
        this.threatIntel = new ThreatIntelligence();
        this.connectionTracker = new ConnectionTracker();
        this.metrics = new MetricsCollector();
        this.running = false;
        
        setupLogging();
    }
    
    private Map<String, FirewallRule> loadRules() {
        Map<String, FirewallRule> ruleMap = new ConcurrentHashMap<>();
        
        // Load from database or file
        try {
            // Example rule loading from JSON file
            Path rulesFile = Paths.get("rules.json");
            if (Files.exists(rulesFile)) {
                String json = Files.readString(rulesFile);
                // Parse JSON and create rules
                // Implementation depends on JSON library used
            }
        } catch (Exception e) {
            logger.log(Level.WARNING, "Failed to load rules", e);
        }
        
        // Add default rules
        addDefaultRules(ruleMap);
        
        return ruleMap;
    }
    
    private void addDefaultRules(Map<String, FirewallRule> ruleMap) {
        // Default deny all
        FirewallRule defaultDeny = new FirewallRule();
        defaultDeny.ruleId = "default-deny";
        defaultDeny.name = "Default Deny All";
        defaultDeny.type = RuleType.DENY;
        defaultDeny.protocol = Protocol.ANY;
        defaultDeny.sourceIp = "ANY";
        defaultDeny.destIp = "ANY";
        defaultDeny.priority = 1000;
        defaultDeny.enabled = true;
        ruleMap.put(defaultDeny.ruleId, defaultDeny);
        
        // Allow loopback
        FirewallRule allowLoopback = new FirewallRule();
        allowLoopback.ruleId = "allow-loopback";
        allowLoopback.name = "Allow Loopback";
        allowLoopback.type = RuleType.ALLOW;
        allowLoopback.protocol = Protocol.ANY;
        allowLoopback.sourceIp = "127.0.0.0/8";
        allowLoopback.destIp = "127.0.0.0/8";
        allowLoopback.priority = 1;
        allowLoopback.enabled = true;
        ruleMap.put(allowLoopback.ruleId, allowLoopback);
    }
    
    private void setupLogging() {
        try {
            Handler fileHandler = new FileHandler("firewall.log", true);
            fileHandler.setFormatter(new SimpleFormatter());
            logger.addHandler(fileHandler);
            
            // Also log to console
            ConsoleHandler consoleHandler = new ConsoleHandler();
            consoleHandler.setLevel(Level.INFO);
            logger.addHandler(consoleHandler);
            
            logger.setLevel(Level.INFO);
        } catch (IOException e) {
            System.err.println("Failed to setup logging: " + e.getMessage());
        }
    }
    
    public void start() throws IOException {
        int port = config.getInt("firewall.port", 9999);
        serverSocket = new ServerSocket(port);
        running = true;
        
        logger.info("Advanced Firewall started on port " + port);
        
        while (running) {
            try {
                Socket clientSocket = serverSocket.accept();
                threadPool.submit(() -> handleConnection(clientSocket));
            } catch (SocketException e) {
                if (running) {
                    logger.log(Level.WARNING, "Socket error", e);
                }
            }
        }
    }
    
    private void handleConnection(Socket socket) {
        try {
            // Extract packet information
            Packet packet = extractPacketInfo(socket);
            
            // Apply firewall rules
            Action action = evaluatePacket(packet);
            
            // Take action
            switch (action) {
                case ALLOW:
                    forwardPacket(socket);
                    break;
                case DROP:
                    socket.close();
                    break;
                case REJECT:
                    sendRejection(socket);
                    break;
                case LOG:
                    logger.info("Logged packet: " + packet);
                    forwardPacket(socket);
                    break;
                case ALERT:
                    sendAlert(packet);
                    socket.close();
                    break;
                case QUARANTINE:
                    quarantinePacket(packet);
                    socket.close();
                    break;
            }
            
            // Update metrics
            metrics.incrementCounter("packets." + action.name().toLowerCase());
            
        } catch (Exception e) {
            logger.log(Level.WARNING, "Error handling connection", e);
        }
    }
    
    private Packet extractPacketInfo(Socket socket) {
        Packet packet = new Packet();
        packet.sourceIp = socket.getInetAddress().getHostAddress();
        packet.destIp = socket.getLocalAddress().getHostAddress();
        packet.sourcePort = socket.getPort();
        packet.destPort = socket.getLocalPort();
        packet.protocol = "TCP"; // Simplified
        packet.timestamp = Instant.now();
        packet.size = 0; // Would need to read packet to determine size
        return packet;
    }
    
    private Action evaluatePacket(Packet packet) {
        // Check threat intelligence
        if (threatIntel.isMaliciousIp(packet.sourceIp)) {
            logger.warning("Blocked malicious IP: " + packet.sourceIp);
            metrics.incrementCounter("threat.malicious_ip_blocked");
            return Action.DROP;
        }
        
        // Check rate limiting
        String rateLimitKey = packet.sourceIp + "-" + packet.destPort;
        if (!rateLimiter.allow(rateLimitKey, 100, Duration.ofSeconds(60))) {
            logger.warning("Rate limit exceeded for: " + packet.sourceIp);
            metrics.incrementCounter("threat.rate_limit_exceeded");
            return Action.DROP;
        }
        
        // Check connection limits
        if (connectionTracker.getConnectionCount(packet.sourceIp) > 1000) {
            logger.warning("Connection limit exceeded for: " + packet.sourceIp);
            metrics.incrementCounter("threat.connection_limit_exceeded");
            return Action.DROP;
        }
        
        // Apply rules (highest priority first)
        List<FirewallRule> matchingRules = rules.values().stream()
            .filter(rule -> rule.matches(packet))
            .sorted(Comparator.comparingInt(r -> r.priority))
            .toList();
        
        if (!matchingRules.isEmpty()) {
            FirewallRule matchedRule = matchingRules.get(0);
            switch (matchedRule.type) {
                case ALLOW: return Action.ALLOW;
                case DENY: return Action.DROP;
                case RATE_LIMIT: return Action.DROP;
                case LOG_ONLY: return Action.LOG;
                case QUARANTINE: return Action.QUARANTINE;
            }
        }
        
        // Default action
        return Action.DROP;
    }
    
    private void forwardPacket(Socket socket) {
        // Implement packet forwarding logic
        // This would typically involve copying data between sockets
    }
    
    private void sendRejection(Socket socket) throws IOException {
        OutputStream out = socket.getOutputStream();
        String rejection = "Connection rejected by firewall\n";
        out.write(rejection.getBytes());
        out.flush();
        socket.close();
    }
    
    private void sendAlert(Packet packet) {
        Map<String, Object> alertData = new HashMap<>();
        alertData.put("source_ip", packet.sourceIp);
        alertData.put("dest_ip", packet.destIp);
        alertData.put("dest_port", packet.destPort);
        alertData.put("timestamp", packet.timestamp);
        
        metrics.addEvent("ALERT", alertData);
        
        // Could send email, SMS, or SIEM integration
        logger.severe("SECURITY ALERT: " + packet);
    }
    
    private void quarantinePacket(Packet packet) {
        // Implement packet quarantine logic
        // Store packet for analysis
        logger.info("Packet quarantined: " + packet);
    }
    
    public void stop() {
        running = false;
        try {
            if (serverSocket != null) {
                serverSocket.close();
            }
        } catch (IOException e) {
            logger.log(Level.WARNING, "Error stopping firewall", e);
        }
        
        threadPool.shutdown();
        try {
            if (!threadPool.awaitTermination(10, TimeUnit.SECONDS)) {
                threadPool.shutdownNow();
            }
        } catch (InterruptedException e) {
            threadPool.shutdownNow();
            Thread.currentThread().interrupt();
        }
        
        logger.info("Firewall stopped");
    }
    
    // Management API
    public void addRule(FirewallRule rule) {
        rules.put(rule.ruleId, rule);
        saveRules();
        logger.info("Added rule: " + rule.name);
    }
    
    public void removeRule(String ruleId) {
        FirewallRule removed = rules.remove(ruleId);
        if (removed != null) {
            saveRules();
            logger.info("Removed rule: " + removed.name);
        }
    }
    
    public List<FirewallRule> getRules() {
        return new ArrayList<>(rules.values());
    }
    
    private void saveRules() {
        // Implement rule persistence
        try {
            // Save to JSON file
            // Implementation depends on JSON library used
        } catch (Exception e) {
            logger.log(Level.WARNING, "Failed to save rules", e);
        }
    }
    
    // REST API for management (using JAX-RS or Spring)
    public static class FirewallAPI {
        // Implement REST endpoints for rule management
    }
}
