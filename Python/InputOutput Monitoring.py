# io_monitor.py - Monitor your own program's I/O
import time

class ProgramMonitor:
    def __init__(self):
        self.start_time = time.time()
        self.operations = []
    
    def log_operation(self, operation, data=""):
        timestamp = time.time() - self.start_time
        self.operations.append({
            'time': timestamp,
            'operation': operation,
            'data': str(data)[:50]  # Limit data length
        })
        print(f"[{timestamp:.2f}s] {operation}: {data}")
    
    def print_summary(self):
        print("\n=== Program Summary ===")
        print(f"Total runtime: {time.time() - self.start_time:.2f} seconds")
        print(f"Total operations: {len(self.operations)}")
        for op in self.operations[-5:]:  # Show last 5 operations
            print(f"  {op['operation']} at {op['time']:.2f}s")

# Usage example
monitor = ProgramMonitor()
monitor.log_operation("Program started")
monitor.log_operation("File read", "sample.txt")
monitor.log_operation("Data processed", "100 records")
monitor.print_summary()
