/**
 * Main entry point for the slide presentation
 */
document.addEventListener('DOMContentLoaded', () => {
    const controller = new SlideController();
    setupKeyboardNavigation(controller);
    setupImageErrorHandling();
});

/**
 * Set up keyboard navigation
 * @param {SlideController} controller - The slide controller
 */
function setupKeyboardNavigation(controller) {
    document.addEventListener('keydown', (e) => {
        switch (e.key) {
            case 'ArrowRight':
            case ' ':
                controller.handleForward();
                break;
            case 'ArrowLeft':
                controller.handleBackward();
                break;
        }
    });
}

/**
 * Set up image error handling
 */
function setupImageErrorHandling() {
    document.querySelectorAll('img').forEach(img => {
        img.onerror = function() {
            this.style.display = 'none';
            const errorText = document.createElement('p');
            errorText.textContent = `(Place your image here: ${this.getAttribute('src')})`;
            errorText.style.color = '#666';
            errorText.style.fontStyle = 'italic';
            this.parentNode.insertBefore(errorText, this.nextSibling);
        };
    });
}