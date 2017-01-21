#pragma once

namespace VVVV 
{
	namespace DataTypes 
	{
		[GuidAttribute("D0FA06A8-E796-46b2-AB21-497009F871BD"),
		InterfaceType(ComInterfaceType::InterfaceIsIUnknown)]
		public interface class IShapeIO: INodeIOBase
		{
			b2Shape* GetSlice(int index);
		};


		public ref class ShapeDataType : IShapeIO 
		{
			private:
				static Guid^ m_guid;
				std::vector<b2Shape*>* m_shapes;
			public:
				ShapeDataType();

				virtual b2Shape* GetSlice(int index);
				void Reset();
				void Add(b2Shape* shape);
				int Count();


				static String^ FriendlyName = "Box2d Shape";
				static property Guid^ GUID 
				{
					Guid^ get() 
					{
						return gcnew System::Guid("D0FA06A8-E796-46b2-AB21-497009F871BD");
					}
				}
		};
	}
}
