/**
 * Animation utility functions
 */
const AnimationUtils = {
    ANIMATIONS: {
        'appear': { type: 'show', class: 'appear', reverseClass: 'disappear' },
        'disappear': { type: 'hide', class: 'disappear', reverseClass: 'reappear' },
        'barrel-roll': { type: 'maintain', class: 'barrel-roll', reverseClass: 'barrel-roll-reverse' }
    },
    
    ANIMATION_CLASSES: [
        'appear', 'disappear', 'reappear', 'barrel-roll', 'barrel-roll-reverse'
    ],
    
    /**
     * Remove all animation classes from an element
     * @param {Element} element - Element to clean
     */
    removeAnimationClasses(element) {
        element.classList.remove(...this.ANIMATION_CLASSES);
    },

    /**
     * Make elements visible (without animation)
     * @param {Array} elements - Elements to make visible
     */
    makeElementsVisible(elements) {
        if (!elements?.length) return;
        elements.forEach(element => {
            const animType = this.getAnimationType(element.dataset.animation);
            if (animType === 'hide') {
                element.style.opacity = '0';
                return;
            }
            element.style.opacity = '1';
            this.removeAnimationClasses(element);
        });
    },

    /**
     * Make elements invisible (without animation)
     * @param {Array} elements - Elements to make invisible
     */
    makeElementsInvisible(elements) {
        if (!elements?.length) return;
        elements.forEach(element => {
            const animType = this.getAnimationType(element.dataset.animation);
            if (animType === 'show') return;
            element.style.opacity = '0';
            this.removeAnimationClasses(element);
        });
    },
    
    /**
     * Get animation type (show, hide, maintain)
     * @param {string} animationName - Name of the animation
     * @returns {string} Animation type
     */
    getAnimationType(animationName) {
        const animation = this.ANIMATIONS[animationName];
        return animation ? animation.type : 'show';
    },

    /**
     * Get animation class based on animation and direction
     * @param {string} animationName - Name of the animation
     * @param {boolean} isAppearing - Whether the element is appearing or disappearing
     * @returns {string} Animation class name
     */
    getAnimationClass(animationName, isAppearing) {
        const animation = this.ANIMATIONS[animationName];
        if (!animation) return isAppearing ? 'appear' : 'disappear';
        if (animation.type === 'hide') {
            if (isAppearing) return animation.class;
            else return animation.reverseClass;
        }
        if (animation.type === 'show') {
            if (isAppearing) return animation.class;
            else return animation.reverseClass;
        }
        if (isAppearing) return animation.class;
        else return animation.reverseClass || animation.class;
    },
    
    /**
     * Get initial opacity based on animation type and direction
     * @param {string} animationName - Name of the animation
     * @param {boolean} isAppearing - Whether the element is appearing or disappearing
     * @returns {string} Initial opacity value
     */
    getInitialOpacity(animationName, isAppearing) {
        const type = this.getAnimationType(animationName);
        if (type === 'hide') return isAppearing ? '1' : '0';
        if (type === 'show') return isAppearing ? '0' : '1';
        return '1';
    },
    
    /**
     * Get final opacity based on animation type and direction
     * @param {string} animationName - Name of the animation
     * @param {boolean} isAppearing - Whether the element is appearing or disappearing
     * @returns {string} Final opacity value
     */
    getFinalOpacity(animationName, isAppearing) {
        const type = this.getAnimationType(animationName);
        if (type === 'hide') return isAppearing ? '0' : '1';
        if (type === 'show') return isAppearing ? '1' : '0';
        return '1';
    },

    /**
     * Animate an element
     * @param {Element} element - Element to animate
     * @param {boolean} isAppearing - Whether the element is appearing or disappearing
     * @returns {Promise} A promise that resolves when the animation is complete
     */
    animateElement(element, isAppearing) {
        return new Promise(resolve => {
            const animationName = element.dataset.animation || (isAppearing ? 'appear' : 'disappear');
            this.removeAnimationClasses(element);
            element.style.opacity = this.getInitialOpacity(animationName, isAppearing);
            const animationClass = this.getAnimationClass(animationName, isAppearing);
            setTimeout(() => {
                element.classList.add(animationClass);
            }, 20);
            element.addEventListener('animationend', () => {
                element.style.opacity = this.getFinalOpacity(animationName, isAppearing);
                resolve();
            }, { once: true });
        });
    },
    
    /**
     * Initialize elements for animation
     * @param {Array} elements - Elements to initialize
     */
    initializeElements(elements) {
        if (!elements?.length) return;
        elements.forEach(element => {
            const animationName = element.dataset.animation || 'appear';
            const type = this.getAnimationType(animationName);
            if (type === 'hide') {
                element.style.opacity = '1';
                return;
            }
            if (type === 'show') {
                element.style.opacity = '0';
                return;
            }
            element.style.opacity = '1';
        });
    }
}; 