#pragma once

using namespace System::Collections::Generic;



namespace VVVV 
{
	namespace DataTypes 
	{
		[GuidAttribute("2104667E-DEF9-402c-BC1A-CF94616B62FC"),
		InterfaceType(ComInterfaceType::InterfaceIsIUnknown)]
		public interface class IShapeDefIO: INodeIOBase
		{
			b2ShapeDef* GetSlice(int index);
			String^ GetCustom(int index);
		};


		public ref class ShapeDefDataType : IShapeDefIO , public IDisposable
		{
			private:
				std::vector<b2ShapeDef*>* m_shapes;
				List<String^>^ m_custom;
			public:

				ShapeDefDataType();
				~ShapeDefDataType();
				virtual b2ShapeDef* GetSlice(int index);
				virtual String^ GetCustom(int index);
				void Reset();

				b2CircleDef* AddCircle();
				b2PolygonDef* AddPolygon();
				b2EdgeChainDef* AddEdgeChain();
				void AddCustom(String^ cust);

				static String^ FriendlyName = "Box2d Shape Definition";
				static property Guid^ GUID 
				{
					Guid^ get() 
					{
						return gcnew System::Guid("2104667E-DEF9-402c-BC1A-CF94616B62FC");
					}
				}
		};
	}
}
