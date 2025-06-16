/**
 * Animation utility functions
 */
const AnimationUtils = {
    ANIMATIONS: {
        'appear': { type: 'show', class: 'appear' },
        'disappear': { type: 'hide', class: 'disappear' },
        'barrel-roll': { type: 'maintain', class: 'barrel-roll', reverseClass: 'barrel-roll-reverse' }
    },
    
    ANIMATION_CLASSES: [
        'appear', 'disappear', 'barrel-roll', 'barrel-roll-reverse'
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
            if (animType === 'hide') return;
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
        const type = animation.type;
        if (type === 'maintain') return isAppearing ? animation.class : (animation.reverseClass || animation.class);
        if ((type === 'show' && isAppearing) || (type === 'hide' && !isAppearing)) return animation.class;
        const oppositeType = type === 'show' ? 'hide' : 'show';
        const oppositeAnim = Object.values(this.ANIMATIONS).find(a => a.type === oppositeType);
        return oppositeAnim ? oppositeAnim.class : (type === 'show' ? 'disappear' : 'appear');
    },
    
    /**
     * Get initial opacity based on animation type and direction
     * @param {string} animationName - Name of the animation
     * @param {boolean} isAppearing - Whether the element is appearing or disappearing
     * @returns {string} Initial opacity value
     */
    getInitialOpacity(animationName, isAppearing) {
        const type = this.getAnimationType(animationName);
        if (type === 'maintain') return '1';
        if (type === 'show' && isAppearing) return '0';
        if (type === 'hide' && !isAppearing) return '1';
        if (type === 'show' && !isAppearing) return '1';
        if (type === 'hide' && isAppearing) return '0';
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
        if (type === 'maintain') return '1';
        if (type === 'show' && isAppearing) return '1';
        if (type === 'hide' && !isAppearing) return '0';
        if (type === 'show' && !isAppearing) return '0';
        if (type === 'hide' && isAppearing) return '1';
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
            const type = this.getAnimationType(animationName);
            const shouldUseOppositeAnimation = (type === 'show' && !isAppearing) || (type === 'hide' && isAppearing);
            this.removeAnimationClasses(element);
            element.style.opacity = this.getInitialOpacity(animationName, isAppearing);
            const animationClass = this.getAnimationClass(animationName, isAppearing);
            setTimeout(() => {
                element.classList.add(animationClass);
            }, 10);
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
            if (type === 'show') element.style.opacity = '0';
            else element.style.opacity = '1';
        });
    }
}; 