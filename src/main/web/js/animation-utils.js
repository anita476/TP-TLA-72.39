/**
 * Animation utility functions
 */
const AnimationUtils = {
    /**
     * Make elements visible (without animation)
     * @param {Array} elements - Elements to make visible
     */
    makeElementsVisible(elements) {
        if (!elements) return;
        elements.forEach(element => {
            element.style.opacity = '1';
            element.classList.remove('appear', 'disappear', 'barrel-roll');
            // Mark barrel-roll elements as completed
            if (element.dataset.animation === 'barrel-roll') {
                element.classList.add('barrel-roll-completed');
            }
        });
    },

    /**
     * Make elements invisible (without animation)
     * @param {Array} elements - Elements to make invisible
     */
    makeElementsInvisible(elements) {
        if (!elements) return;
        elements.forEach(element => {
            element.style.opacity = '0';
            element.classList.remove('appear', 'disappear', 'barrel-roll', 'barrel-roll-completed');
        });
    },

    /**
     * Animate an element
     * @param {Element} element - Element to animate
     * @param {boolean} isAppearing - Whether the element is appearing or disappearing
     * @returns {Promise} A promise that resolves when the animation is complete
     */
    animateElement(element, isAppearing) {
        return new Promise(resolve => {
            const animation = element.dataset.animation || (isAppearing ? 'appear' : 'disappear');
            
            // Remove all animation classes
            element.classList.remove('appear', 'disappear', 'barrel-roll', 'barrel-roll-completed');
            
            // Special handling for barrel-roll
            if (animation === 'barrel-roll') {
                if (isAppearing) {
                    element.style.opacity = '1'; // Always visible for barrel-roll
                } else {
                    // For disappearing, we need to reverse the barrel-roll
                    // but keep the element visible
                    element.style.opacity = '1'; // Keep visible
                    element.style.transform = 'rotate(360deg)';
                    setTimeout(() => {
                        element.style.transition = 'transform 0.8s linear';
                        element.style.transform = 'rotate(0deg)';
                    }, 10);
                    
                    setTimeout(() => {
                        // Don't change opacity, keep the element visible
                        element.style.transform = '';
                        element.style.transition = '';
                        resolve();
                    }, 810);
                    return;
                }
            } else if (isAppearing && (animation === 'appear' || !animation)) {
                element.style.opacity = '0';
            }
            
            // Add the appropriate animation class
            element.classList.add(isAppearing ? animation : 'disappear');
            
            element.addEventListener('animationend', () => {
                // For non-barrel-roll animations, set opacity based on appearing/disappearing
                if (animation !== 'barrel-roll') {
                    element.style.opacity = isAppearing ? '1' : '0';
                } else {
                    // For barrel-roll, always keep visible
                    element.style.opacity = '1';
                }
                
                // For barrel-roll, mark as completed after animation
                if (animation === 'barrel-roll' && isAppearing) {
                    element.classList.remove('barrel-roll');
                    element.classList.add('barrel-roll-completed');
                }
                
                resolve();
            }, { once: true });
        });
    },
    
    /**
     * Initialize elements for animation
     * @param {Array} elements - Elements to initialize
     */
    initializeElements(elements) {
        elements.forEach(element => {
            if (element.dataset.animation === 'appear' || !element.dataset.animation) {
                element.style.opacity = '0';
            } else if (element.dataset.animation === 'barrel-roll') {
                element.style.opacity = '1';
            }
        });
    }
}; 