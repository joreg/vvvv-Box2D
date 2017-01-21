#pragma once
#include "../DataTypes/WorldDataType.h"
#include "../DataTypes/BodyDataType.h"
#include "../DataTypes/JointDataType.h"
#include "../DataTypes/GroundDataType.h"
#include "../Internals/Contact/ContactListener.h"
#include "../Internals/Engine/ObjectManager.h"

#include "../Internals/Data/BodyCustomData.h"
#include "../Internals/Data/ShapeCustomData.h"
#include "../Internals/Data/JointCustomData.h"
#include "../Internals/Data/ContactResultData.h"

using namespace VVVV::DataTypes;

namespace VVVV 
{
	namespace Nodes 
	{
		public ref class Box2dWorldNode : IPlugin,public IDisposable
		{
			private:
				IPluginHost^ FHost;

				IValueIn^ vInLowerBound;
				IValueIn^ vInUpperBound;
				IValueIn^ vInGravity;
				IValueIn^ vInAllowSleep;
				IValueIn^ vInEnabled;
				IValueFastIn^ vInTimeStep;
				IValueFastIn^ vInPosIterations;
				IValueFastIn^ vInVelIterations;
				IValueIn^ vInReset;

				IValueOut^ vOutWorldValid;
				IValueOut^ vOutControllerCount;
				INodeOut^ vOutWorldNode;
				INodeOut^ vOutBodies;
				INodeOut^ vOutGround;
				INodeOut^ vOutJoints;
				IValueOut^ vOutReset;

				WorldDataType^ mWorld;
				BodyDataType^ mBodies;
				GroundDataType^ mGround;
				JointDataType^ mJoints;

				b2World* internalworld;

				bool ctrlconnected;
				
				std::vector<ContactReportId*>* reports;
				std::vector<ContactResultData*>* results;
				std::vector<b2Controller*>* controller;
				ContactListener* MyListener;
			
			public:
				static property IPluginInfo^ PluginInfo 
				{
					IPluginInfo^ get() 
					{
						//IPluginInfo^ Info;
						IPluginInfo^ Info = gcnew VVVV::PluginInterfaces::V1::PluginInfo();
						Info->Name = "World";
						Info->Category = "Box2d";
						Info->Version = "";
						Info->Help = "Box2d World definition";
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




				Box2dWorldNode(void);
				 ~Box2dWorldNode();

				virtual void SetPluginHost(IPluginHost^ Host);
				virtual void Configurate(IPluginConfig^ Input);
				virtual void Evaluate(int SpreadMax);
				
				virtual property bool AutoEvaluate 
				{
					bool get() { return false; }
				}



		};
	}
}

