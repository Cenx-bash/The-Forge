// ==============================================
// YOUTUBE MUSIC PLAYER SYSTEM
// ==============================================
// System to play music from YouTube URLs
// Note: Requires additional libraries for full functionality

#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <regex>
#include <fstream>
#include <sstream>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
#endif

namespace fs = std::filesystem;

// ==============================================
// CONFIGURATION AND CONSTANTS
// ==============================================

const std::string DEFAULT_SONG = "https://www.youtube.com/watch?v=rxXsdj7EBm4";
const std::string YOUTUBE_DL_PATH = "yt-dlp";  // Modern youtube-dl alternative
const std::string FFMPEG_PATH = "ffmpeg";      // For audio conversion
const std::string DOWNLOAD_DIR = "music_cache/";
const std::string PLAYLIST_FILE = "playlist.txt";

// ==============================================
// AUDIO PLAYER INTERFACE
// ==============================================

class AudioPlayer {
public:
    virtual ~AudioPlayer() = default;
    virtual bool play(const std::string& filepath) = 0;
    virtual void stop() = 0;
    virtual bool isPlaying() const = 0;
    virtual void setVolume(int volume) = 0; // 0-100
};

// ==============================================
// PLATFORM-SPECIFIC AUDIO PLAYERS
// ==============================================

#ifdef _WIN32
// Windows Audio Player using Windows Media
class WindowsAudioPlayer : public AudioPlayer {
private:
    bool playing = false;
    std::string currentFile;
    
public:
    bool play(const std::string& filepath) override {
        stop();
        
        std::wstring widePath(filepath.begin(), filepath.end());
        std::wstring command = L"open \"" + widePath + L"\" type mpegvideo alias mp3";
        
        // Using MCI (Media Control Interface) - works for WAV and MP3
        std::wstring mciCommand = L"play \"" + widePath + L"\"";
        
        // Alternative: Use system call with default player
        std::string sysCommand = "start /B \"\" \"" + filepath + "\"";
        
        if(system(sysCommand.c_str()) == 0) {
            currentFile = filepath;
            playing = true;
            return true;
        }
        
        return false;
    }
    
    void stop() override {
        if(!currentFile.empty()) {
            std::string command = "taskkill /F /IM wmplayer.exe >nul 2>&1";
            system(command.c_str());
            playing = false;
        }
    }
    
    bool isPlaying() const override {
        return playing;
    }
    
    void setVolume(int volume) override {
        // Windows volume control would require additional code
        // This is a simplified version
        std::cout << "[Windows] Volume set to: " << volume << "%\n";
    }
};
#else
// Linux/Mac Audio Player
class UnixAudioPlayer : public AudioPlayer {
private:
    pid_t playerPid = 0;
    bool playing = false;
    
public:
    bool play(const std::string& filepath) override {
        stop();
        
        // Try different players in order of preference
        std::vector<std::string> players = {"mpg123", "ffplay", "mplayer", "vlc"};
        
        for(const auto& player : players) {
            std::string command = "which " + player + " > /dev/null 2>&1";
            if(system(command.c_str()) == 0) {
                // Found the player, use it
                std::string playCommand = player + " \"" + filepath + "\" > /dev/null 2>&1 &";
                playerPid = fork();
                
                if(playerPid == 0) {
                    // Child process
                    execlp("sh", "sh", "-c", playCommand.c_str(), NULL);
                    exit(0);
                } else if(playerPid > 0) {
                    playing = true;
                    return true;
                }
            }
        }
        
        return false;
    }
    
    void stop() override {
        if(playerPid > 0) {
            kill(playerPid, SIGTERM);
            playerPid = 0;
            playing = false;
        }
    }
    
    bool isPlaying() const override {
        return playing;
    }
    
    void setVolume(int volume) override {
        // PulseAudio volume control (Linux)
        std::string command = "pactl set-sink-volume @DEFAULT_SINK@ " + 
                              std::to_string(volume) + "% > /dev/null 2>&1";
        system(command.c_str());
    }
};
#endif

// ==============================================
// YOUTUBE DOWNLOADER
// ==============================================

class YouTubeDownloader {
private:
    std::string extractVideoId(const std::string& url) {
        std::regex pattern(R"((?:youtube\.com\/watch\?v=|youtu\.be\/)([^&\s]+))");
        std::smatch matches;
        
        if(std::regex_search(url, matches, pattern) && matches.size() > 1) {
            return matches[1].str();
        }
        return "";
    }
    
    std::string sanitizeFilename(const std::string& filename) {
        std::string result = filename;
        std::replace_if(result.begin(), result.end(), 
                       [](char c) { return !std::isalnum(c) && c != ' ' && c != '-' && c != '_'; }, 
                       '_');
        return result;
    }
    
public:
    std::string downloadAudio(const std::string& youtubeUrl, int quality = 0) {
        std::string videoId = extractVideoId(youtubeUrl);
        if(videoId.empty()) {
            std::cerr << "Invalid YouTube URL\n";
            return "";
        }
        
        // Create cache directory if it doesn't exist
        if(!fs::exists(DOWNLOAD_DIR)) {
            fs::create_directory(DOWNLOAD_DIR);
        }
        
        // Check if already downloaded
        std::string cachedFile = DOWNLOAD_DIR + videoId + ".mp3";
        if(fs::exists(cachedFile)) {
            std::cout << "Using cached audio: " << cachedFile << "\n";
            return cachedFile;
        }
        
        // Download using yt-dlp
        std::string command = YOUTUBE_DL_PATH + 
                             " -x --audio-format mp3" +
                             " -o \"" + DOWNLOAD_DIR + "%(id)s.%(ext)s\"" +
                             " \"" + youtubeUrl + "\"";
        
        std::cout << "Downloading audio...\n";
        std::cout << "Command: " << command << "\n";
        
        int result = system(command.c_str());
        
        if(result == 0 && fs::exists(cachedFile)) {
            std::cout << "Download complete: " << cachedFile << "\n";
            return cachedFile;
        }
        
        // Fallback: Try with youtube-dl
        command = "youtube-dl -x --audio-format mp3" +
                 " -o \"" + DOWNLOAD_DIR + "%(id)s.%(ext)s\"" +
                 " \"" + youtubeUrl + "\"";
        
        result = system(command.c_str());
        
        if(result == 0) {
            // Check for any mp3 file with the video ID
            for(const auto& entry : fs::directory_iterator(DOWNLOAD_DIR)) {
                if(entry.path().string().find(videoId) != std::string::npos &&
                   entry.path().extension() == ".mp3") {
                    return entry.path().string();
                }
            }
        }
        
        return "";
    }
    
    std::string getAudioStreamUrl(const std::string& youtubeUrl) {
        // This method gets the direct audio stream URL without downloading
        std::string command = YOUTUBE_DL_PATH + 
                             " -g -f bestaudio" +
                             " \"" + youtubeUrl + "\" 2>/dev/null";
        
        std::string result;
        char buffer[128];
        
        #ifdef _WIN32
            FILE* pipe = _popen(command.c_str(), "r");
        #else
            FILE* pipe = popen(command.c_str(), "r");
        #endif
        
        if(pipe) {
            while(fgets(buffer, sizeof(buffer), pipe) != NULL) {
                result += buffer;
            }
            
            #ifdef _WIN32
                _pclose(pipe);
            #else
                pclose(pipe);
            #endif
        }
        
        // Clean up newlines
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
        
        return result;
    }
};

// ==============================================
// PLAYLIST MANAGER
// ==============================================

class PlaylistManager {
private:
    std::vector<std::string> playlist;
    size_t currentIndex = 0;
    
public:
    void loadFromFile(const std::string& filename) {
        playlist.clear();
        std::ifstream file(filename);
        std::string line;
        
        while(std::getline(file, line)) {
            if(!line.empty() && line[0] != '#') {
                playlist.push_back(line);
            }
        }
        
        if(!playlist.empty()) {
            std::cout << "Loaded " << playlist.size() << " songs from playlist\n";
        }
    }
    
    void addSong(const std::string& url) {
        playlist.push_back(url);
        saveToFile(PLAYLIST_FILE);
    }
    
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        for(const auto& song : playlist) {
            file << song << "\n";
        }
    }
    
    std::string getCurrent() const {
        if(currentIndex < playlist.size()) {
            return playlist[currentIndex];
        }
        return "";
    }
    
    std::string getNext() {
        if(playlist.empty()) return "";
        
        currentIndex = (currentIndex + 1) % playlist.size();
        return playlist[currentIndex];
    }
    
    std::string getPrevious() {
        if(playlist.empty()) return "";
        
        currentIndex = (currentIndex == 0) ? playlist.size() - 1 : currentIndex - 1;
        return playlist[currentIndex];
    }
    
    void shuffle() {
        std::random_shuffle(playlist.begin(), playlist.end());
        currentIndex = 0;
    }
    
    void listAll() const {
        std::cout << "\n=== PLAYLIST ===\n";
        for(size_t i = 0; i < playlist.size(); i++) {
            std::cout << (i + 1) << ". " 
                     << (i == currentIndex ? "[NOW PLAYING] " : "")
                     << playlist[i] << "\n";
        }
        std::cout << "================\n";
    }
    
    size_t size() const {
        return playlist.size();
    }
    
    void clear() {
        playlist.clear();
        currentIndex = 0;
    }
};

// ==============================================
// MAIN PLAYER SYSTEM
// ==============================================

class YouTubeMusicPlayer {
private:
    std::unique_ptr<AudioPlayer> audioPlayer;
    YouTubeDownloader downloader;
    PlaylistManager playlist;
    std::string currentAudioFile;
    int volume = 80;
    
    void displayBanner() {
        system(CLEAR_SCREEN);
        std::cout << R"(
  __  __           _        __  __           _       _ 
 |  \/  |_   _ ___| |_ ___ |  \/  |_   _ ___| |_ ___| |
 | |\/| | | | / __| __/ _ \| |\/| | | | / __| __/ _ \ |
 | |  | | |_| \__ \ || (_) | |  | | |_| \__ \ ||  __/ |
 |_|  |_|\__,_|___/\__\___/|_|  |_|\__,_|___/\__\___|_|
                                                        
        )" << "\n";
        
        std::cout << "Currently playing: " 
                  << (currentAudioFile.empty() ? "Nothing" : currentAudioFile) 
                  << "\n";
        std::cout << "Volume: " << volume << "%\n\n";
    }
    
    void displayMenu() {
        std::cout << "=== MENU ===\n";
        std::cout << "1. Play specific YouTube URL\n";
        std::cout << "2. Play default song (Aaj Ke Baad)\n";
        std::cout << "3. Play from playlist\n";
        std::cout << "4. Add to playlist\n";
        std::cout << "5. Show playlist\n";
        std::cout << "6. Shuffle playlist\n";
        std::cout << "7. Volume Up (+10)\n";
        std::cout << "8. Volume Down (-10)\n";
        std::cout << "9. Stop playback\n";
        std::cout << "10. Stream directly (experimental)\n";
        std::cout << "0. Exit\n";
        std::cout << "=============\n";
        std::cout << "Choice: ";
    }
    
    void playUrl(const std::string& url) {
        std::cout << "Processing: " << url << "\n";
        
        // Method 1: Download and play
        std::string audioFile = downloader.downloadAudio(url);
        
        if(!audioFile.empty()) {
            std::cout << "Playing: " << audioFile << "\n";
            if(audioPlayer->play(audioFile)) {
                currentAudioFile = audioFile;
                std::cout << "Playback started!\n";
            } else {
                std::cout << "Failed to play audio\n";
            }
        } else {
            std::cout << "Failed to download audio\n";
            
            // Method 2: Try direct streaming
            std::cout << "Attempting direct stream...\n";
            std::string streamUrl = downloader.getAudioStreamUrl(url);
            
            if(!streamUrl.empty()) {
                std::cout << "Stream URL obtained\n";
                // Note: Direct streaming would require a player that supports HTTP streams
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
public:
    YouTubeMusicPlayer() {
        #ifdef _WIN32
            audioPlayer = std::make_unique<WindowsAudioPlayer>();
        #else
            audioPlayer = std::make_unique<UnixAudioPlayer>();
        #endif
        
        // Load existing playlist
        if(fs::exists(PLAYLIST_FILE)) {
            playlist.loadFromFile(PLAYLIST_FILE);
        }
        
        // Add default song to playlist if empty
        if(playlist.size() == 0) {
            playlist.addSong(DEFAULT_SONG);
        }
    }
    
    void run() {
        int choice;
        
        do {
            displayBanner();
            displayMenu();
            std::cin >> choice;
            std::cin.ignore(); // Clear newline
            
            switch(choice) {
                case 1: {
                    std::string url;
                    std::cout << "Enter YouTube URL: ";
                    std::getline(std::cin, url);
                    playUrl(url);
                    break;
                }
                    
                case 2:
                    std::cout << "Playing default song...\n";
                    playUrl(DEFAULT_SONG);
                    break;
                    
                case 3:
                    if(playlist.size() > 0) {
                        std::string current = playlist.getCurrent();
                        std::cout << "Playing from playlist...\n";
                        playUrl(current);
                    } else {
                        std::cout << "Playlist is empty!\n";
                    }
                    break;
                    
                case 4: {
                    std::string url;
                    std::cout << "Enter YouTube URL to add: ";
                    std::getline(std::cin, url);
                    playlist.addSong(url);
                    std::cout << "Added to playlist!\n";
                    break;
                }
                    
                case 5:
                    playlist.listAll();
                    std::cout << "Press Enter to continue...";
                    std::cin.get();
                    break;
                    
                case 6:
                    playlist.shuffle();
                    std::cout << "Playlist shuffled!\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    break;
                    
                case 7:
                    volume = std::min(100, volume + 10);
                    audioPlayer->setVolume(volume);
                    break;
                    
                case 8:
                    volume = std::max(0, volume - 10);
                    audioPlayer->setVolume(volume);
                    break;
                    
                case 9:
                    audioPlayer->stop();
                    currentAudioFile.clear();
                    std::cout << "Playback stopped\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    break;
                    
                case 10: {
                    std::cout << "Experimental streaming mode\n";
                    std::string streamUrl = downloader.getAudioStreamUrl(DEFAULT_SONG);
                    if(!streamUrl.empty()) {
                        std::cout << "Stream URL: " << streamUrl << "\n";
                        // This would require a player that can handle HTTP streams
                        std::cout << "Opening in browser...\n";
                        #ifdef _WIN32
                            std::string cmd = "start \"\" \"" + streamUrl + "\"";
                        #else
                            std::string cmd = "xdg-open \"" + streamUrl + "\"";
                        #endif
                        system(cmd.c_str());
                    }
                    break;
                }
                    
                case 0:
                    audioPlayer->stop();
                    std::cout << "Goodbye!\n";
                    break;
                    
                default:
                    std::cout << "Invalid choice!\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
        } while(choice != 0);
    }
    
    void autoPlayDefault() {
        std::cout << "Auto-playing default song...\n";
        playUrl(DEFAULT_SONG);
        
        // Keep playing for 30 seconds
        std::this_thread::sleep_for(std::chrono::seconds(30));
        
        audioPlayer->stop();
    }
};

// ==============================================
// SIMPLIFIED VERSION - EASY TO COMPILE
// ==============================================

class SimpleYouTubePlayer {
public:
    void playDirect() {
        std::cout << "🎵 Playing 'Aaj Ke Baad' by Mohit Chauhan 🎵\n";
        std::cout << "===========================================\n\n";
        
        // Method 1: Open in default browser
        std::cout << "Opening in your web browser...\n";
        std::string url = "https://www.youtube.com/watch?v=rxXsdj7EBm4";
        
        #ifdef _WIN32
            std::string command = "start \"\" \"" + url + "\"";
        #elif __APPLE__
            std::string command = "open \"" + url + "\"";
        #else
            std::string command = "xdg-open \"" + url + "\"";
        #endif
        
        system(command.c_str());
        
        // Method 2: Download and play if tools are available
        std::cout << "\nChecking for audio tools...\n";
        
        // Check for yt-dlp or youtube-dl
        if(system("which yt-dlp > /dev/null 2>&1") == 0 ||
           system("which youtube-dl > /dev/null 2>&1") == 0) {
            
            std::cout << "Downloading audio...\n";
            std::string dlCommand = "yt-dlp -x --audio-format mp3 --no-playlist -o \"temp_audio.%(ext)s\" \"" + url + "\"";
            system(dlCommand.c_str());
            
            // Try to play the downloaded file
            #ifdef _WIN32
                system("start temp_audio.mp3");
            #elif __APPLE__
                system("afplay temp_audio.mp3 &");
            #else
                system("mpg123 temp_audio.mp3 &");
            #endif
            
            std::cout << "Audio playback started!\n";
        } else {
            std::cout << "For best experience, install:\n";
            std::cout << "1. yt-dlp: https://github.com/yt-dlp/yt-dlp\n";
            std::cout << "2. ffmpeg: https://ffmpeg.org/\n";
            std::cout << "3. mpg123 or VLC for audio playback\n";
        }
        
        std::cout << "\nEnjoy the music! Press Enter to exit...\n";
        std::cin.get();
    }
};

// ==============================================
// MAIN FUNCTION
// ==============================================

int main() {
    std::cout << "Select mode:\n";
    std::cout << "1. Simple mode (just play in browser)\n";
    std::cout << "2. Advanced mode (requires yt-dlp/ffmpeg)\n";
    std::cout << "Choice: ";
    
    int mode;
    std::cin >> mode;
    
    if(mode == 1) {
        SimpleYouTubePlayer simplePlayer;
        simplePlayer.playDirect();
    } else {
        YouTubeMusicPlayer player;
        player.run();
    }
    
    return 0;
}
