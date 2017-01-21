#pragma once
#include "../../DataTypes/BodyDataType.h"

using namespace VVVV::DataTypes;

namespace VVVV 
{
	namespace Nodes 
	{
		public ref class Box2dUpdateBodyNode : IPlugin,IPluginConnections
		{
		public:
			Box2dUpdateBodyNode(void);

			static property IPluginInfo^ PluginInfo 
				{
					IPluginInfo^ get() 
					{
						//IPluginInfo^ Info;
						IPluginInfo^ Info = gcnew VVVV::PluginInterfaces::V1::PluginInfo();
						Info->Name = "UpdateBody";
						Info->Category = "Box2d";
						Info->Version = "";
						Info->Help = "Update box2d body";
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

			virtual void SetPluginHost(IPluginHost^ Host);
			virtual void Configurate(IPluginConfig^ Input);
			virtual void Evaluate(int SpreadMax);
			virtual void ConnectPin(IPluginIO^ Pin);
			virtual void DisconnectPin(IPluginIO^ Pin);
			
			virtual property bool AutoEvaluate 
			{
				bool get() { return true; }
			}

		private:
			IPluginHost^ FHost;

			INodeIn^ vInBodies;
			BodyDataType^ m_bodies;

			IValueIn^ vInPosition;
			IValueIn^ vInSetPosition;

			IValueIn^ vInAngle;
			IValueIn^ vInSetAngle;

			IValueIn^ vInVelocity;
			IValueIn^ vInSetVelocity;

			IValueIn^ vInAngularVelocity;
			IValueIn^ vInSetAngularVelocity;

			IStringIn^ vInCustom;
			IValueIn^ vInSetCustom;

			IValueIn^ vInSleeping;
			IValueIn^ vInSetSleeping;

			IValueIn^ vInHasTTL;
			IValueIn^ vInTTL;
			IValueIn^ vInSetTTL;
			


		};
	}
}
