"""
Main LLM Chat Application
"""

import os
import sys
from typing import Optional, Dict, Any
from dotenv import load_dotenv

# Load environment variables
load_dotenv()

class LLMChatApplication:
    """Main application class for LLM chat"""
    
    def __init__(self):
        self.config = self._load_config()
        self.auth_manager = None
        self.database = None
        self.current_user = None
        self.current_conversation = None
        
        # Import modules after environment is loaded
        self._initialize_modules()
    
    def _load_config(self) -> Dict[str, Any]:
        """Load application configuration"""
        return {
            'debug': os.getenv('DEBUG', 'False').lower() == 'true',
            'default_model': os.getenv('DEFAULT_MODEL', 'gpt-4'),
            'default_temperature': float(os.getenv('DEFAULT_TEMPERATURE', 0.7)),
            'max_tokens': int(os.getenv('MAX_TOKENS', 4096)),
            'api_keys': {
                'openai': os.getenv('OPENAI_API_KEY'),
                'anthropic': os.getenv('ANTHROPIC_API_KEY')
            }
        }
    
    def _initialize_modules(self):
        """Initialize application modules"""
        try:
            from auth import auth_manager
            from chat_database import get_database
            
            self.auth_manager = auth_manager
            self.database = get_database()
            
        except ImportError as e:
            print(f"Error importing modules: {e}")
            sys.exit(1)
    
    def check_api_keys(self) -> bool:
        """Check if required API keys are set"""
        missing_keys = []
        
        if not self.config['api_keys']['openai']:
            missing_keys.append('OPENAI_API_KEY')
        
        if not self.config['api_keys']['anthropic']:
            missing_keys.append('ANTHROPIC_API_KEY')
        
        if missing_keys:
            print(f"⚠️  Warning: Missing API keys: {', '.join(missing_keys)}")
            print("Some features may not work without these keys.")
            return False
        
        return True
    
    def run_cli_mode(self):
        """Run in command-line interface mode"""
        print("\n" + "="*60)
        print("🤖 LLM Chat Application - CLI Mode")
        print("="*60 + "\n")
        
        # Check API keys
        self.check_api_keys()
        
        # Import and run CLI
        try:
            from bin import main as cli_main
            cli_main()
        except ImportError as e:
            print(f"Error running CLI: {e}")
            sys.exit(1)
    
    def run_web_mode(self):
        """Run in web application mode (stub for future implementation)"""
        print("\n" + "="*60)
        print("🌐 LLM Chat Application - Web Mode")
        print("="*60 + "\n")
        
        print("Web mode is not implemented yet.")
        print("Run 'python main.py cli' for command-line interface.")
        print("Or run 'python bin.py' for direct CLI access.")
    
    def run(self, mode: str = 'cli'):
        """Run the application in specified mode"""
        if mode == 'cli':
            self.run_cli_mode()
        elif mode == 'web':
            self.run_web_mode()
        else:
            print(f"Unknown mode: {mode}")
            print("Available modes: cli, web")


def main():
    """Main entry point"""
    import argparse
    
    parser = argparse.ArgumentParser(description='LLM Chat Application')
    parser.add_argument('mode', nargs='?', default='cli', choices=['cli', 'web'],
                       help='Application mode (cli or web)')
    parser.add_argument('--version', '-v', action='store_true',
                       help='Show application version')
    
    args = parser.parse_args()
    
    if args.version:
        print("LLM Chat Application v1.0.0")
        return
    
    # Create and run application
    app = LLMChatApplication()
    app.run(args.mode)


if __name__ == '__main__':
    main()
