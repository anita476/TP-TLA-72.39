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
            if (element.dataset.animation === 'disappear') {
                return;
            }
            
            element.style.opacity = '1';
            element.classList.remove('appear', 'disappear', 'barrel-roll');
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
            
            element.classList.remove('appear', 'disappear', 'barrel-roll', 'barrel-roll-completed');
            
            if (animation === 'barrel-roll') {
                if (isAppearing) {
                    element.style.opacity = '1';
                    element.classList.add('barrel-roll');
                } else {
                    element.style.opacity = '1';
                    element.style.transform = 'rotate(360deg)';
                    setTimeout(() => {
                        element.style.transition = 'transform 0.8s linear';
                        element.style.transform = 'rotate(0deg)';
                    }, 10);
                    
                    setTimeout(() => {
                        element.style.transform = '';
                        element.style.transition = '';
                        resolve();
                    }, 810);
                    return;
                }
            } else if (animation === 'disappear') {
                if (isAppearing) {
                    element.style.opacity = '0';
                    setTimeout(() => {
                        element.classList.add('appear');
                    }, 10);
                } else {
                    element.style.opacity = '1';
                    setTimeout(() => {
                        element.classList.add('disappear');
                    }, 10);
                }
            } else {
                // Default handling for appear and other animations
                if (isAppearing) {
                    element.style.opacity = '0';
                    element.classList.add('appear');
                } else {
                    element.style.opacity = '1';
                    element.classList.add('disappear');
                }
            }
            
            element.addEventListener('animationend', () => {
                if (animation !== 'barrel-roll') {
                    element.style.opacity = isAppearing ? '1' : '0';
                } else {
                    element.style.opacity = '1';
                }
                
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
            const animation = element.dataset.animation || 'appear';
            
            if (animation === 'appear' || !animation) {
                element.style.opacity = '0';
            } else if (animation === 'barrel-roll') {
                element.style.opacity = '1';
            } else if (animation === 'disappear') {
                element.style.opacity = '1';
            }
        });
    }
}; 