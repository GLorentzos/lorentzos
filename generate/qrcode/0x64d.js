// Define function to generate QR code
function generateQRCode() {
    var inputLink = document.getElementById('qrcode').value; // Get input link
    var qrContainer = document.getElementById('qrcode-container');
    var qrCanvas = document.createElement('canvas');
    var errorDisplay = document.getElementById('error');
  
    // Check if input link is empty
    if (!inputLink) {
      alert('Please enter a valid link.');
      return;
    }
  
    // Clear previous error messages
    errorDisplay.innerHTML = '';
  
    try {
      // Generate QR code
      var qrCode = new QRCode(qrCanvas, {
        text: inputLink,
        width: 128,
        height: 128
      });
  
      // Display QR code image
      qrContainer.innerHTML = '';
      var qrImage = qrCanvas.toDataURL('image/png');
      qrContainer.appendChild(qrCanvas);
  
      // Show QR code and scroll to it
      qrContainer.style.display = 'block';
      qrContainer.scrollIntoView({ behavior: 'smooth' });
    } catch (error) {
      console.error('Error generating QR code:', error);
      alert('Error generating QR code. Please try again.');
    }
  }
  