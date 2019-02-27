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
    /// Horizontal layout that will position its child elements left to right.
    /// </summary>
    public sealed class GUILayoutX : GUILayout
    {
        /// <summary>
        /// Constructs a new empty horizontal layout.
        /// </summary>
        public GUILayoutX()
        {
            Internal_CreateInstanceX(this, new GUIOption[0]);
        }

        /// <summary>
        /// Constructs a new empty horizontal layout.
        /// </summary>
        /// <param name="options">Options that allow you to control how is the layout positioned and sized.</param>
        public GUILayoutX(params GUIOption[] options)
        {
            Internal_CreateInstanceX(this, options);
        }
    }

    /** @} */
}
