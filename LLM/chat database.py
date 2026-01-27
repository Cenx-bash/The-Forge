"""
Database module for storing chat conversations and user data
"""

import sqlite3
import json
from datetime import datetime
from typing import List, Dict, Any, Optional, Tuple
import os
from dotenv import load_dotenv

load_dotenv()

class ChatDatabase:
    """Manages SQLite database for chat storage"""
    
    def __init__(self, db_path: str = None):
        self.db_path = db_path or os.getenv('DATABASE_URL', 'sqlite:///chat_database.db').replace('sqlite:///', '')
        self.connection = None
        self.connect()
    
    def connect(self) -> None:
        """Establish database connection"""
        try:
            self.connection = sqlite3.connect(self.db_path)
            self.connection.row_factory = sqlite3.Row
        except sqlite3.Error as e:
            print(f"Database connection error: {e}")
            raise
    
    def initialize_database(self) -> None:
        """Create database tables if they don't exist"""
        cursor = self.connection.cursor()
        
        # Users table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS users (
                user_id TEXT PRIMARY KEY,
                username TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                last_login TIMESTAMP
            )
        ''')
        
        # Conversations table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS conversations (
                conversation_id TEXT PRIMARY KEY,
                user_id TEXT,
                title TEXT DEFAULT 'New Conversation',
                model_used TEXT,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users (user_id)
            )
        ''')
        
        # Messages table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS messages (
                message_id TEXT PRIMARY KEY,
                conversation_id TEXT,
                role TEXT NOT NULL,
                content TEXT NOT NULL,
                tokens INTEGER,
                model TEXT,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                metadata TEXT,
                FOREIGN KEY (conversation_id) REFERENCES conversations (conversation_id)
            )
        ''')
        
        # Create indexes for better performance
        cursor.execute('CREATE INDEX IF NOT EXISTS idx_conversations_user ON conversations(user_id)')
        cursor.execute('CREATE INDEX IF NOT EXISTS idx_messages_conversation ON messages(conversation_id)')
        cursor.execute('CREATE INDEX IF NOT EXISTS idx_messages_timestamp ON messages(timestamp)')
        
        self.connection.commit()
    
    def save_message(self, conversation_id: str, role: str, content: str, 
                    model: str = None, tokens: int = None, metadata: dict = None) -> str:
        """Save a message to the database"""
        cursor = self.connection.cursor()
        message_id = f"msg_{datetime.utcnow().strftime('%Y%m%d%H%M%S%f')}"
        
        metadata_json = json.dumps(metadata) if metadata else None
        
        cursor.execute('''
            INSERT INTO messages (message_id, conversation_id, role, content, tokens, model, metadata)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (message_id, conversation_id, role, content, tokens, model, metadata_json))
        
        # Update conversation timestamp
        cursor.execute('''
            UPDATE conversations 
            SET updated_at = CURRENT_TIMESTAMP
            WHERE conversation_id = ?
        ''', (conversation_id,))
        
        self.connection.commit()
        return message_id
    
    def create_conversation(self, user_id: str, title: str = None, 
                           model_used: str = None) -> str:
        """Create a new conversation"""
        cursor = self.connection.cursor()
        conversation_id = f"conv_{datetime.utcnow().strftime('%Y%m%d%H%M%S%f')}"
        
        title = title or f"Conversation {datetime.utcnow().strftime('%Y-%m-%d %H:%M')}"
        
        cursor.execute('''
            INSERT INTO conversations (conversation_id, user_id, title, model_used)
            VALUES (?, ?, ?, ?)
        ''', (conversation_id, user_id, title, model_used))
        
        self.connection.commit()
        return conversation_id
    
    def get_conversation_messages(self, conversation_id: str) -> List[Dict[str, Any]]:
        """Retrieve all messages from a conversation"""
        cursor = self.connection.cursor()
        cursor.execute('''
            SELECT message_id, role, content, tokens, model, timestamp, metadata
            FROM messages
            WHERE conversation_id = ?
            ORDER BY timestamp ASC
        ''', (conversation_id,))
        
        messages = []
        for row in cursor.fetchall():
            message = dict(row)
            if message['metadata']:
                message['metadata'] = json.loads(message['metadata'])
            messages.append(message)
        
        return messages
    
    def get_user_conversations(self, user_id: str, limit: int = 50) -> List[Dict[str, Any]]:
        """Get all conversations for a user"""
        cursor = self.connection.cursor()
        cursor.execute('''
            SELECT conversation_id, title, model_used, created_at, updated_at
            FROM conversations
            WHERE user_id = ?
            ORDER BY updated_at DESC
            LIMIT ?
        ''', (user_id, limit))
        
        return [dict(row) for row in cursor.fetchall()]
    
    def update_conversation_title(self, conversation_id: str, title: str) -> bool:
        """Update conversation title"""
        cursor = self.connection.cursor()
        cursor.execute('''
            UPDATE conversations
            SET title = ?
            WHERE conversation_id = ?
        ''', (title, conversation_id))
        
        self.connection.commit()
        return cursor.rowcount > 0
    
    def delete_conversation(self, conversation_id: str) -> bool:
        """Delete a conversation and all its messages"""
        cursor = self.connection.cursor()
        
        # Delete messages first (due to foreign key constraint)
        cursor.execute('DELETE FROM messages WHERE conversation_id = ?', (conversation_id,))
        
        # Delete conversation
        cursor.execute('DELETE FROM conversations WHERE conversation_id = ?', (conversation_id,))
        
        self.connection.commit()
        return cursor.rowcount > 0
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get database statistics"""
        cursor = self.connection.cursor()
        
        stats = {}
        
        # Total conversations
        cursor.execute('SELECT COUNT(*) as count FROM conversations')
        stats['total_conversations'] = cursor.fetchone()['count']
        
        # Total messages
        cursor.execute('SELECT COUNT(*) as count FROM messages')
        stats['total_messages'] = cursor.fetchone()['count']
        
        # Users
        cursor.execute('SELECT COUNT(*) as count FROM users')
        stats['users'] = cursor.fetchone()['count']
        
        # Models used
        cursor.execute('SELECT DISTINCT model FROM messages WHERE model IS NOT NULL')
        stats['models_used'] = [row['model'] for row in cursor.fetchall()]
        
        return stats
    
    def clear_all_chats(self) -> None:
        """Clear all chat data (for testing/reset purposes)"""
        cursor = self.connection.cursor()
        cursor.execute('DELETE FROM messages')
        cursor.execute('DELETE FROM conversations')
        self.connection.commit()
    
    def close(self) -> None:
        """Close database connection"""
        if self.connection:
            self.connection.close()


# Database singleton
db_instance = None

def get_database() -> ChatDatabase:
    """Get or create database instance"""
    global db_instance
    if db_instance is None:
        db_instance = ChatDatabase()
        db_instance.initialize_database()
    return db_instance
