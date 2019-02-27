//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Data about a sub-mesh range and the type of primitives contained in the range.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct SubMesh
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static SubMesh Default()
		{
			SubMesh value = new SubMesh();
			value.indexOffset = 0;
			value.indexCount = 0;
			value.drawOp = MeshTopology.TriangleList;

			return value;
		}

		public SubMesh(int indexOffset, int indexCount, MeshTopology drawOp)
		{
			this.indexOffset = indexOffset;
			this.indexCount = indexCount;
			this.drawOp = drawOp;
		}

		public int indexOffset;
		public int indexCount;
		public MeshTopology drawOp;
	}

	/** @} */
}
