from flask import Flask, render_template, request, redirect, url_for, flash
import os
import hashlib
import base64
import secrets
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.backends import default_backend

app = Flask(__name__)
app.secret_key = 'your_secret_key'

DATABASE_FOLDER = 'database'


def encrypt_password(password, salt):
    # Encrypt password with SHA-256 hash
    return hashlib.sha256((password + salt).encode()).hexdigest()


def generate_user_keys(username_folder):
    # Generate RSA key pair
    private_key = rsa.generate_private_key(public_exponent=65537, key_size=2048, backend=default_backend())
    public_key = private_key.public_key()

    # Save the private key
    with open(os.path.join(username_folder, 'private_key.pem'), 'wb') as f:
        f.write(
            private_key.private_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PrivateFormat.TraditionalOpenSSL,
                encryption_algorithm=serialization.NoEncryption()
            )
        )

    # Save the public key
    with open(os.path.join(username_folder, 'public_key.pem'), 'wb') as f:
        f.write(
            public_key.public_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PublicFormat.SubjectPublicKeyInfo
            )
        )


@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        
        # Check if username already exists
        user_folder = os.path.join(DATABASE_FOLDER, username)
        if os.path.isdir(user_folder):
            flash('Username already exists, please choose another one.', 'error')
            return redirect(url_for('register'))
        
        # Generate a unique salt for the user
        salt = base64.urlsafe_b64encode(secrets.token_bytes(16)).decode()

        # Create user folder
        os.makedirs(user_folder, exist_ok=True)
        
        # Encrypt the password
        encrypted_password = encrypt_password(password, salt)

        # Save password and salt
        with open(os.path.join(user_folder, 'user.password'), 'w') as f:
            f.write(encrypted_password)
        
        with open(os.path.join(user_folder, 'salt'), 'w') as f:
            f.write(salt)
        
        # Generate RSA keys and save them in user's folder
        generate_user_keys(user_folder)
        
        # Create storage folder
        os.makedirs(os.path.join(user_folder, 'storage'), exist_ok=True)

        flash('User registered successfully!', 'success')
        return redirect(url_for('login'))
    return render_template('register.html')


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        
        # Check if user folder exists
        user_folder = os.path.join(DATABASE_FOLDER, username)
        if not os.path.isdir(user_folder):
            flash('Username does not exist.', 'error')
            return redirect(url_for('login'))
        
        # Read salt and encrypted password
        with open(os.path.join(user_folder, 'salt'), 'r') as f:
            salt = f.read()
        
        with open(os.path.join(user_folder, 'user.password'), 'r') as f:
            stored_password = f.read()

        # Check password
        if stored_password == encrypt_password(password, salt):
            flash('Login successful!', 'success')
            return redirect(url_for('dashboard'))
        else:
            flash('Incorrect password.', 'error')
    return render_template('login.html')


@app.route('/dashboard')
def dashboard():
    return "Welcome to the Dashboard!"


if __name__ == '__main__':
    # Create the database folder if it doesn't exist
    os.makedirs(DATABASE_FOLDER, exist_ok=True)
    app.run(debug=True)
