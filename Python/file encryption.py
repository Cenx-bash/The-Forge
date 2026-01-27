# simple_crypto.py - Learn encryption fundamentals
import base64

def simple_encode(text):
    """Basic encoding for learning purposes"""
    encoded = base64.b64encode(text.encode()).decode()
    return encoded

def simple_decode(encoded_text):
    """Basic decoding for learning purposes"""
    decoded = base64.b64decode(encoded_text.encode()).decode()
    return decoded

# Example usage
message = "Hello, this is a secret message!"
encoded = simple_encode(message)
decoded = simple_decode(encoded)

print("=== Encryption Learning ===")
print(f"Original: {message}")
print(f"Encoded: {encoded}")
print(f"Decoded: {decoded}")
print(f"Match: {message == decoded}")
