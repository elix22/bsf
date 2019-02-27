﻿//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;

namespace bs
{
    /** @addtogroup GUI_Engine
     *  @{
     */

    /// <summary>
    /// Vertical layout that will position its child elements top to bottom.
    /// </summary>
    public sealed class GUILayoutY : GUILayout
    {
        /// <summary>
        /// Internal method used by the runtime. Initializes a managed version of the vertical layout that is referenced
        /// by a native GUI scroll area element.
        /// </summary>
        /// <param name="parentArea">Scroll area of which we want to reference the layout of.</param>
        internal GUILayoutY(GUIScrollArea parentArea)
        {
            Internal_CreateInstanceYFromScrollArea(this, parentArea);
        }

        /// <summary>
        /// Constructs a new empty vertical layout.
        /// </summary>
        public GUILayoutY()
        {
            Internal_CreateInstanceY(this, new GUIOption[0]);
        }

        /// <summary>
        /// Constructs a new empty vertical layout.
        /// </summary>
        public GUILayoutY(params GUIOption[] options)
        {
            Internal_CreateInstanceY(this, options);
        }
    }

    /** @} */
}
