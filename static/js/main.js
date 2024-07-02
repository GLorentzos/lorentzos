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

// TRANSLATE

document.addEventListener('DOMContentLoaded', function() {
    const select = document.getElementById('language-select');

    select.addEventListener('change', function() {
        const selectedLanguage = select.value;

        if (selectedLanguage === 'gr') {
            translateToGreek();
        } else {
            translateToEnglish();
        }
    });

    function translateToGreek() {
        document.documentElement.lang = 'el'; 

        document.querySelector('h1').textContent = 'Γιώργος Λορέντζος';
        document.querySelector('p').textContent = 'Γεια σου! Ειδικεύομαι στην ανάπτυξη μοντέρνων εφαρμογών UI και backend ως full-stack προγραμματιστής. Με στέρεη βάση σε C, C++ και Python, φέρνω ένα ευέλικτο σύνολο δεξιοτήτων για τη δημιουργία αξιόπιστων και αποδοτικών λύσεων λογισμικού. Αναπτύσσω επίσης ιστοσελίδες, δημιουργώντας ενστικτώδεις διεπαφές χρήστη και βελτιστοποιώντας τη λειτουργικότητα του backend. Απολαμβάνω την παροχή ομαλών εμπειριών που υπερβαίνουν τις προσδοκίες.';
        document.querySelector('.entrusted-me-title').textContent = 'Εταιρείες που με εμπιστεύτηκαν';
    }

    function translateToEnglish() {
        document.documentElement.lang = 'en'; 

        document.querySelector('h1').textContent = 'George Lorentzos';
        document.querySelector('p').textContent = 'Hey there! I specialize in developing modern UI and backend applications as a full-stack developer. With a solid foundation in C, C++, and Python, I bring a versatile skill set to create robust and efficient software solutions. I also build websites, crafting intuitive user interfaces and optimizing backend functionality. I thrive on delivering seamless experiences that exceed expectations.';
        document.querySelector('.entrusted-me-title').textContent = 'Companies That Entrusted Me';
    }
});
