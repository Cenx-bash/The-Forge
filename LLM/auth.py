"""
Authentication and user management module
"""

import os
import hashlib
import secrets
from datetime import datetime, timedelta
from typing import Optional, Dict, Any
import jwt
from dotenv import load_dotenv

load_dotenv()

class UserAuth:
    """Handles user authentication and authorization"""
    
    def __init__(self):
        self.secret_key = os.getenv('SECRET_KEY', 'fallback-secret-key')
        self.session_timeout = int(os.getenv('SESSION_TIMEOUT', 3600))
        self.users = {}  # In production, use database
        
    def hash_password(self, password: str) -> str:
        """Hash password using SHA-256 with salt"""
        salt = secrets.token_hex(16)
        hashed = hashlib.sha256((password + salt).encode()).hexdigest()
        return f"{salt}${hashed}"
    
    def verify_password(self, password: str, hashed_password: str) -> bool:
        """Verify password against hash"""
        try:
            salt, stored_hash = hashed_password.split('$')
            test_hash = hashlib.sha256((password + salt).encode()).hexdigest()
            return secrets.compare_digest(test_hash, stored_hash)
        except:
            return False
    
    def create_token(self, user_id: str, username: str) -> str:
        """Create JWT token for authenticated user"""
        payload = {
            'user_id': user_id,
            'username': username,
            'exp': datetime.utcnow() + timedelta(seconds=self.session_timeout)
        }
        return jwt.encode(payload, self.secret_key, algorithm='HS256')
    
    def verify_token(self, token: str) -> Optional[Dict[str, Any]]:
        """Verify JWT token and return payload if valid"""
        try:
            payload = jwt.decode(token, self.secret_key, algorithms=['HS256'])
            return payload
        except jwt.ExpiredSignatureError:
            return None
        except jwt.InvalidTokenError:
            return None
    
    def register_user(self, username: str, password: str) -> Dict[str, Any]:
        """Register a new user"""
        if username in self.users:
            return {'success': False, 'message': 'Username already exists'}
        
        user_id = secrets.token_hex(8)
        hashed_password = self.hash_password(password)
        
        self.users[username] = {
            'user_id': user_id,
            'username': username,
            'password_hash': hashed_password,
            'created_at': datetime.utcnow().isoformat()
        }
        
        token = self.create_token(user_id, username)
        
        return {
            'success': True,
            'user_id': user_id,
            'username': username,
            'token': token
        }
    
    def login_user(self, username: str, password: str) -> Dict[str, Any]:
        """Authenticate user and return token"""
        user_data = self.users.get(username)
        
        if not user_data:
            return {'success': False, 'message': 'User not found'}
        
        if self.verify_password(password, user_data['password_hash']):
            token = self.create_token(user_data['user_id'], username)
            return {
                'success': True,
                'user_id': user_data['user_id'],
                'username': username,
                'token': token
            }
        
        return {'success': False, 'message': 'Invalid password'}


# Singleton instance
auth_manager = UserAuth()
