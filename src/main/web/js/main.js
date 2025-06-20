document.addEventListener('DOMContentLoaded', () => {
  PresentationCore.init({
    defaultAnimation: 'appear',
    defaultTransition: 'fade'
  });
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