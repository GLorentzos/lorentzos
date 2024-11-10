from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend
import base64

# Password for encryption (use the password that was used to encrypt the private key)
PASSWORD = "1234"  # Replace this with your actual password

# Load the private key from the file
def load_private_key(private_key_path, password, salt):
    # Derive encryption key from the password and salt using PBKDF2
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100000,
        backend=default_backend()
    )
    encryption_key = kdf.derive(password.encode())
    
    # Load private key with password decryption
    with open(private_key_path, "rb") as private_file:
        private_key_pem = private_file.read()
    
    private_key = serialization.load_pem_private_key(private_key_pem, password=encryption_key, backend=default_backend())
    
    return private_key

# Decrypt the encrypted text
def decrypt_with_private_key(private_key, encrypted_text):
    encrypted_data = base64.b64decode(encrypted_text)
    
    decrypted_data = private_key.decrypt(
        encrypted_data,
        padding.OAEP(
            mgf=padding.MGF1(algorithm=hashes.SHA256()),
            algorithm=hashes.SHA256(),
            label=None
        )
    )
    
    return decrypted_data.decode()

# Main function to load files and decrypt
def main():
    # Load the salt from the file
    with open("salt.bin", "rb") as salt_file:
        salt = salt_file.read()

    # Load the private key using password and salt
    private_key = load_private_key("private_key.pem", PASSWORD, salt)

    # Encrypted text to decrypt (replace this with your actual encrypted text)
    encrypted_text = "pa5UQwZPmHrxcfjH3fw/YVwvu2n3JBG5ZWHSwgqwxgpCwV6TcWBREyx2eWuUYLwsQEEe8+2+P2DpQYejvnXVu24hN0yzavX6b0qZ7+J6hm87bIGBNxZo/PIM0WMKVmIEahqv9qS75Gn5h9Lz77/rGYl+DfMZAPNy7vbZkSeFTrm7U4nur2rVI3rAzoe1p1637ulm1EQi/4szu3Qfx8q0xzOz3s7WgCWzGZ83ZPonn0FZxu+VTRdoRDE2TRsBM1MH6aTvFAhZI8h3FFGT2zgwuaeVDuJvlIDAXJgAfABKsGsBE5V0EOPu8pfC3zJfoEpihnMvKOgHshtqlil0X02tCw=="  # Replace with your encrypted text

    # Decrypt the text
    decrypted_text = decrypt_with_private_key(private_key, encrypted_text)
    
    print(f"Decrypted Text: {decrypted_text}")

if __name__ == "__main__":
    main()
