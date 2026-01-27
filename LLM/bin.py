#!/usr/bin/env python3
"""
Command-line interface for the LLM Chat Application
"""

import argparse
import sys
import os
from typing import List, Optional
from dotenv import load_dotenv

load_dotenv()

class ChatCLI:
    """Command Line Interface for chat operations"""
    
    def __init__(self):
        self.parser = self._create_parser()
        
    def _create_parser(self) -> argparse.ArgumentParser:
        """Create argument parser with all commands"""
        parser = argparse.ArgumentParser(
            description='LLM Chat Application - Command Line Interface',
            epilog='Example: python bin.py chat --message "Hello, AI!"'
        )
        
        subparsers = parser.add_subparsers(dest='command', help='Available commands')
        
        # Chat command
        chat_parser = subparsers.add_parser('chat', help='Start a chat session')
        chat_parser.add_argument('--message', '-m', type=str, help='Direct message to send')
        chat_parser.add_argument('--model', type=str, default=os.getenv('DEFAULT_MODEL', 'gpt-4'),
                               help='Model to use for chat')
        chat_parser.add_argument('--stream', '-s', action='store_true',
                               help='Stream response in real-time')
        
        # Auth commands
        auth_parser = subparsers.add_parser('auth', help='Authentication operations')
        auth_subparsers = auth_parser.add_subparsers(dest='auth_command')
        
        register_parser = auth_subparsers.add_parser('register', help='Register new user')
        register_parser.add_argument('username', type=str, help='Username')
        register_parser.add_argument('password', type=str, help='Password')
        
        login_parser = auth_subparsers.add_parser('login', help='Login user')
        login_parser.add_argument('username', type=str, help='Username')
        login_parser.add_argument('password', type=str, help='Password')
        
        # Database commands
        db_parser = subparsers.add_parser('db', help='Database operations')
        db_parser.add_argument('action', choices=['init', 'clear', 'stats'],
                             help='Database action to perform')
        
        # Config command
        config_parser = subparsers.add_parser('config', help='Configuration operations')
        config_parser.add_argument('--show', action='store_true', help='Show current configuration')
        config_parser.add_argument('--set', nargs=2, metavar=('KEY', 'VALUE'),
                                 help='Set configuration value')
        
        return parser
    
    def handle_chat(self, args) -> None:
        """Handle chat command"""
        print(f"Starting chat with model: {args.model}")
        
        if args.message:
            print(f"Your message: {args.message}")
            
            # Simulate AI response
            print("\nAI Response:")
            print("-" * 50)
            print(f"Hello! You said: '{args.message}'")
            print("How can I assist you further today?")
            print("-" * 50)
        else:
            print("Interactive mode - type 'exit' to quit")
            while True:
                try:
                    user_input = input("\nYou: ").strip()
                    if user_input.lower() in ['exit', 'quit', 'bye']:
                        print("Goodbye!")
                        break
                    
                    if args.stream:
                        print("\nAI: ", end='', flush=True)
                        # Simulate streaming
                        response = f"I received: '{user_input}'. Let me think about that..."
                        for char in response:
                            print(char, end='', flush=True)
                            import time
                            time.sleep(0.02)
                        print()
                    else:
                        print(f"\nAI: I received: '{user_input}'. How can I help you?")
                        
                except KeyboardInterrupt:
                    print("\n\nChat interrupted. Goodbye!")
                    break
    
    def handle_auth(self, args) -> None:
        """Handle authentication commands"""
        from auth import auth_manager
        
        if args.auth_command == 'register':
            result = auth_manager.register_user(args.username, args.password)
            if result['success']:
                print(f"✅ User '{args.username}' registered successfully!")
                print(f"Token: {result['token'][:50]}...")
            else:
                print(f"❌ Registration failed: {result['message']}")
                
        elif args.auth_command == 'login':
            result = auth_manager.login_user(args.username, args.password)
            if result['success']:
                print(f"✅ Login successful! Welcome {args.username}")
                print(f"Token: {result['token'][:50]}...")
            else:
                print(f"❌ Login failed: {result['message']}")
    
    def handle_db(self, args) -> None:
        """Handle database commands"""
        from chat_database import ChatDatabase
        
        db = ChatDatabase()
        
        if args.action == 'init':
            db.initialize_database()
            print("✅ Database initialized successfully!")
        elif args.action == 'clear':
            confirm = input("Are you sure you want to clear all chat history? (yes/no): ")
            if confirm.lower() == 'yes':
                db.clear_all_chats()
                print("✅ All chat history cleared!")
            else:
                print("Operation cancelled.")
        elif args.action == 'stats':
            stats = db.get_statistics()
            print("\n📊 Database Statistics:")
            print(f"Total conversations: {stats['total_conversations']}")
            print(f"Total messages: {stats['total_messages']}")
            print(f"Users: {stats['users']}")
            print(f"Models used: {', '.join(stats['models_used'])}")
    
    def handle_config(self, args) -> None:
        """Handle configuration commands"""
        if args.show:
            print("\n🔧 Current Configuration:")
            print("-" * 40)
            for key, value in os.environ.items():
                if key.startswith(('OPENAI', 'ANTHROPIC', 'DEFAULT', 'MAX')):
                    print(f"{key}: {value}")
        
        if args.set:
            key, value = args.set
            print(f"Setting {key} = {value}")
            # In a real app, you would update the .env file here
            print("Note: To persist changes, edit the .env file directly.")
    
    def run(self, args: Optional[List[str]] = None) -> None:
        """Run the CLI with given arguments"""
        parsed_args = self.parser.parse_args(args)
        
        if not parsed_args.command:
            self.parser.print_help()
            return
        
        try:
            if parsed_args.command == 'chat':
                self.handle_chat(parsed_args)
            elif parsed_args.command == 'auth':
                self.handle_auth(parsed_args)
            elif parsed_args.command == 'db':
                self.handle_db(parsed_args)
            elif parsed_args.command == 'config':
                self.handle_config(parsed_args)
        except Exception as e:
            print(f"❌ Error: {e}")
            sys.exit(1)


def main():
    """Main entry point for CLI"""
    cli = ChatCLI()
    cli.run()


if __name__ == '__main__':
    main()
