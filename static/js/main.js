// PRELOADER

window.addEventListener('load', function() {
    setTimeout(function() {
        handlePreloader();
    }, 1000);
});

function handlePreloader() {
    var preloader = document.querySelector('.preloader');
    if (!preloader) return;
    
    preloader.style.opacity = '0';
    setTimeout(function() {
        preloader.parentNode.removeChild(preloader);
    }, 500);
    
    document.body.classList.add('page-loaded');
}
