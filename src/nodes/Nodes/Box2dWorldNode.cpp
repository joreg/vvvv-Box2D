
#include "Box2dWorldNode.h"


namespace VVVV 
{
	namespace Nodes 
	{
		Box2dWorldNode::Box2dWorldNode(void) 
		{
			this->mWorld = gcnew v4b2d::WorldDataType();
			this->mBodies = gcnew v4b2d::BodyDataType();
			this->mGround = gcnew v4b2d::GroundDataType();
			this->mJoints = gcnew v4b2d::JointDataType();
			
			this->reports = new std::vector<ContactReportId*>();
			this->results = new std::vector<ContactResultData*>();
			this->MyListener = new ContactListener(this->results,this->reports);
			//this->mWorld->Reports = this->reports;
			//this->mWorld->Results = this->results;
			this->ctrlconnected = false;
		}

		Box2dWorldNode::~Box2dWorldNode() 
		{
			if (this->internalworld != nullptr) 
			{
				delete this->internalworld;
			}
		}

		void Box2dWorldNode::SetPluginHost(v4::IPluginHost^ Host)
		{
			this->FHost = Host;

			//Bounds
			this->FHost->CreateValueInput("Lower Bound",2,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInLowerBound);
			this->vInLowerBound->SetSubType2D(System::Double::MinValue,System::Double::MaxValue,0.01,-100.0,-100.0,false,false,false);

			this->FHost->CreateValueInput("Upper Bound",2,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInUpperBound);
			this->vInUpperBound->SetSubType2D(System::Double::MinValue,System::Double::MaxValue,0.01,100.0,100.0,false,false,false);

			//Gravity
			this->FHost->CreateValueInput("Gravity",2,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInGravity);
			this->vInGravity->SetSubType2D(System::Double::MinValue,System::Double::MaxValue,0.01,0,-1.0,false,false,false);

			this->FHost->CreateValueFastInput("Time Step",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vInTimeStep);
			this->vInTimeStep->SetSubType(0,System::Double::MaxValue,0.01,0.01,false,false,false);

			this->FHost->CreateValueFastInput("Position Iterations",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vInPosIterations);
			this->vInPosIterations->SetSubType(1,System::Double::MaxValue,1,8,false,false,true);

			this->FHost->CreateValueFastInput("Velocity Iterations",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vInVelIterations);
			this->vInVelIterations->SetSubType(1,System::Double::MaxValue,1,10,false,false,true);

			//Allow to put objects in sleep mode
			this->FHost->CreateValueInput("Allow Sleep",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vInAllowSleep);
			this->vInAllowSleep->SetSubType(0,1,1,1,false,true,false);

			//Is World Enabled
			this->FHost->CreateValueInput("Enabled",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vInEnabled);
			this->vInEnabled->SetSubType(0,1,1,0,false,true,false);

			this->FHost->CreateValueInput("Reset",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vInReset);
			this->vInReset->SetSubType(0,1,1,0,true,false,false);



			//World output
			this->FHost->CreateNodeOutput("World",v4::TSliceMode::Single,v4::TPinVisibility::True,this->vOutWorldNode);
			this->vOutWorldNode->SetSubType(VVVV::Utils::ArrayUtils::SingleGuidArray(v4b2d::WorldDataType::GUID), v4b2d::WorldDataType::FriendlyName);
			this->vOutWorldNode->SetInterface(this->mWorld);

			this->FHost->CreateValueOutput("Controller Count",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vOutControllerCount);
			this->vOutControllerCount->SetSubType(0,System::Double::MaxValue,1,0,false,false,true);

			this->FHost->CreateValueOutput("World Valid",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vOutWorldValid);
			this->vOutWorldValid->SetSubType(0,1,1,0,false,true,false);

			this->FHost->CreateNodeOutput("Ground",v4::TSliceMode::Single,v4::TPinVisibility::True,this->vOutGround);
			this->vOutGround->SetSubType(VVVV::Utils::ArrayUtils::SingleGuidArray(v4b2d::GroundDataType::GUID), v4b2d::GroundDataType::FriendlyName);
			this->vOutGround->SetInterface(this->mGround);

			this->FHost->CreateNodeOutput("Bodies",v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutBodies);
			this->vOutBodies->SetSubType(VVVV::Utils::ArrayUtils::SingleGuidArray(v4b2d::BodyDataType::GUID), v4b2d::BodyDataType::FriendlyName);
			this->vOutBodies->SetInterface(this->mBodies);

			this->FHost->CreateNodeOutput("Joints",v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutJoints);
			this->vOutJoints->SetSubType(VVVV::Utils::ArrayUtils::SingleGuidArray(v4b2d::JointDataType::GUID), v4b2d::JointDataType::FriendlyName);
			this->vOutJoints->SetInterface(this->mJoints);

			this->FHost->CreateValueOutput("Has Reset",1,nullptr,v4::TSliceMode::Single,v4::TPinVisibility::True,this->vOutReset);
			this->vOutReset->SetSubType(0,1,1,0,true,false,false);
		}


		void Box2dWorldNode::Configurate(v4::IPluginConfig^ Input) 
		{
			
		}

		
		void Box2dWorldNode::Evaluate(int SpreadMax) 
		{	
			double reset;
			this->vInReset->GetValue(0,reset);

			this->mWorld->Reset = false;

			//Reset World
			if (this->vInAllowSleep->PinIsChanged 
				|| this->vInLowerBound->PinIsChanged
				|| this->vInUpperBound->PinIsChanged
				|| reset >= 0.5) 
			{
				if (this->internalworld != nullptr) 
				{
					ObjectManager::ProcessBodies(this->internalworld,this->mWorld->DeletedIds,true,0);
					ObjectManager::ProcessJoints(this->internalworld,true,0);
					delete this->internalworld;
				}

				b2AABB worldAABB;
				double lbx,lby,ubx,uby,gx,gy,allowsleep;
				this->vInAllowSleep->GetValue(0, allowsleep);
				this->vInGravity->GetValue2D(0,gx,gy);
				this->vInLowerBound->GetValue2D(0,lbx,lby);
				this->vInUpperBound->GetValue2D(0,ubx,uby);

				worldAABB.lowerBound.Set(System::Convert::ToSingle(lbx), System::Convert::ToSingle(lby));
				worldAABB.upperBound.Set(System::Convert::ToSingle(ubx), System::Convert::ToSingle(uby));

				this->mWorld->SetIsValid(worldAABB.IsValid());

				if (this->mWorld->GetIsValid()) 
				{
					b2Vec2 gravity(System::Convert::ToSingle(gx), System::Convert::ToSingle(gy));
					bool dosleep = allowsleep >= 0.5;
					this->internalworld  = new b2World(worldAABB, gravity, dosleep);

					this->mWorld->SetWorld(this->internalworld);
					this->mWorld->Results = this->results;
					this->mWorld->Reports = this->reports;
					this->internalworld->SetContactListener(this->MyListener);

					BodyCustomData* gdata = new BodyCustomData();
					gdata->Id = -1;
					gdata->MarkedForDeletion = false;
					gdata->Custom = "";
					this->internalworld->GetGroundBody()->SetUserData(gdata);

					this->mGround->SetGround(this->internalworld->GetGroundBody());
					this->mGround->SetIsValid(true);
					this->mWorld->SetReset(true);				
				} 
				else 
				{
					this->internalworld = nullptr;
					this->mWorld->SetWorld(this->internalworld);
					this->mGround->SetIsValid(false);
					this->mGround->SetGround(nullptr);
					this->mWorld->SetReset(true);
				}

				this->mWorld->Reset = true;

				this->vOutWorldValid->SetValue(0, System::Convert::ToDouble(this->mWorld->GetIsValid()));

			}
			else
			{
				this->mWorld->SetReset(false);
			}

			this->vOutReset->SetValue(0, System::Convert::ToDouble(this->mWorld->Reset));

			if (this->vInGravity->PinIsChanged) 
			{
				double gx,gy;
				this->vInGravity->GetValue2D(0,gx,gy);

				b2Vec2 gravity(System::Convert::ToSingle(gx), System::Convert::ToSingle(gy));
				this->internalworld->SetGravity(gravity);
			}

			//Enabled Change
			if (this->vInEnabled->PinIsChanged) 
			{
				double enabled;
				this->vInEnabled->GetValue(0, enabled);

				this->mWorld->SetIsEnabled(enabled >= 0.5);
			}

			//Process if enabled
			this->mBodies->Reset();
			this->mJoints->Reset();


			this->MyListener->ResetResults();

			//Delete bodies marked as such
			if (this->mWorld->GetIsValid()) 
			{
				double ts = 0;
				this->vInTimeStep->GetValue(0,ts);

				ObjectManager::ProcessBodies(this->internalworld,this->mWorld->DeletedIds,false,ts);
				ObjectManager::ProcessJoints(this->internalworld,false,ts);

				if (this->mWorld->GetIsEnabled()) 
				{
					double pit,vit;

					this->vInPosIterations->GetValue(0,pit);
					this->vInVelIterations->GetValue(0,vit);
					
					this->internalworld->Step(System::Convert::ToSingle(ts), System::Convert::ToInt32(vit), System::Convert::ToInt32(pit));
				}



				this->vOutBodies->MarkPinAsChanged();
				this->vOutBodies->SliceCount = this->mWorld->GetWorld()->GetBodyCount() -1;
				for (b2Body* b = this->mWorld->GetWorld()->GetBodyList(); b; b = b->GetNext())
				{
					if (b != this->mWorld->GetWorld()->GetGroundBody()) 
					{
						this->mBodies->Add(b);
					}
				}

				this->vOutJoints->MarkPinAsChanged();
				this->vOutJoints->SliceCount = this->mWorld->GetWorld()->GetJointCount();
				for (b2Joint* j = this->mWorld->GetWorld()->GetJointList(); j; j = j->GetNext())
				{
					this->mJoints->Add(j);
				}

				this->vOutControllerCount->SetValue(0, this->internalworld->GetControllerCount());
			} 
			else 
			{
				this->vOutControllerCount->SetValue(0,-1);
				this->vOutBodies->SliceCount = 0;
			}
		}			
	}
}