#pragma once
#include "../DataTypes/WorldDataType.h"
#include "../DataTypes/Shapes/ShapeDataType.h"
#include "../DataTypes/BodyDataType.h"
#include "../Internals/Data/ShapeCustomData.h"
#include "../Internals/Data/BodyCustomData.h"

#include "../Utils/ArrayUtils.h"

namespace v4 = VVVV::PluginInterfaces::V1;
namespace gen = System::Collections::Generic;
namespace v4b2d = VVVV::DataTypes;



namespace VVVV 
{
	namespace Nodes 
	{
		public ref class Box2dAABBQueryNode : v4::IPlugin, v4::IPluginConnections
		{
		public:
			Box2dAABBQueryNode(void);
			
			virtual void SetPluginHost(v4::IPluginHost^ Host);
			virtual void Configurate(v4::IPluginConfig^ Input);
			virtual void Evaluate(int SpreadMax);
			virtual void ConnectPin(v4::IPluginIO^ Pin);
			virtual void DisconnectPin(v4::IPluginIO^ Pin);
			
			virtual property bool AutoEvaluate 
			{
				bool get() { return false; }
			}

			static property v4::IPluginInfo^ PluginInfo
			{
				v4::IPluginInfo^ get()
					{
						//v4::IPluginInfo^ Info;
						v4::IPluginInfo^ Info = gcnew VVVV::PluginInterfaces::V1::PluginInfo();
						Info->Name = "AABBQuery";
						Info->Category = "Box2d";
						Info->Version = "";
						Info->Help = "Get all shapes from an aabb query";
						Info->Bugs = "";
						Info->Credits = "Box2d";
						Info->Warnings = "";
						Info->Author = "vux";
						Info->Tags="Physics,2d,Collision";

						//leave below as is
						System::Diagnostics::StackTrace^ st = gcnew System::Diagnostics::StackTrace(true);
						System::Diagnostics::StackFrame^ sf = st->GetFrame(0);
						System::Reflection::MethodBase^ method = sf->GetMethod();
						Info->Namespace = method->DeclaringType->Namespace;
						Info->Class = method->DeclaringType->Name;
						return Info;
					}
			}



		private:
			v4::IPluginHost^ FHost;

			v4::INodeIn^ vInWorld;
			v4b2d::WorldDataType^ m_world;

			v4::IValueIn^ vInLowerBound;
			v4::IValueIn^ vInUpperBound;
			v4::IValueIn^ vInDoQuery;

			v4::IValueOut^ vOutQueryIndex;

			v4::INodeOut^ vOutShapes;
			v4b2d::ShapeDataType^ mShapes;
			v4::IValueOut^ vOutShapeId;
			
			v4::INodeOut^ vOutBodies;
			v4b2d::BodyDataType^ mBodies;
			v4::IValueOut^ vOutBodyId;
			
		};
	}
}
