#include "StdAfx.h"
#include "Box2dCreateBodyNode.h"
#include "../../Internals/Data/BodyCustomData.h"
#include "../../Internals/Data/ShapeCustomData.h"

namespace VVVV 
{
	namespace Nodes 
	{
		Box2dCreateBodyNode::Box2dCreateBodyNode(void)
		{
			this->mBodies = gcnew BodyDataType();
		}

		void Box2dCreateBodyNode::SetPluginHost(IPluginHost^ Host) 
		{
			this->FHost = Host;

			//World input
			this->FHost->CreateNodeInput("World",TSliceMode::Single,TPinVisibility::True,this->vInWorld);
			this->vInWorld->SetSubType(ArrayUtils::SingleGuidArray(WorldDataType::GUID),WorldDataType::FriendlyName);

			this->FHost->CreateNodeInput("Shapes",TSliceMode::Dynamic,TPinVisibility::True,this->vInShapes);
			this->vInShapes->SetSubType(ArrayUtils::SingleGuidArray(ShapeDefDataType::GUID),ShapeDefDataType::FriendlyName);

			//Position and velocity
			this->FHost->CreateValueInput("Position",2,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInPosition);
			this->vInPosition->SetSubType2D(Double::MinValue,Double::MaxValue,0.01,0.0,0.0,false,false,false);

			this->FHost->CreateValueInput("Angle",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInAngle);
			this->vInAngle->SetSubType(Double::MinValue,Double::MaxValue,0.01,0.0,false,false,false);

			this->FHost->CreateValueInput("Velocity",2,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInVelocity);
			this->vInVelocity->SetSubType2D(Double::MinValue,Double::MaxValue,0.01,0.0,0.0,false,false,false);

			this->FHost->CreateValueInput("Angular Velocity",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInAngularVelocity);
			this->vInAngularVelocity->SetSubType(Double::MinValue,Double::MaxValue,0.01,0.0,false,false,false);

			this->FHost->CreateValueInput("Linear Damping",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInLinearDamping);
			this->vInLinearDamping->SetSubType(Double::MinValue,Double::MaxValue,0.01,0.0,false,false,false);

			this->FHost->CreateValueInput("Angular Damping",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInAngularDamping);
			this->vInAngularDamping->SetSubType(Double::MinValue,Double::MaxValue,0.01,0.0,false,false,false);

			this->FHost->CreateValueInput("Fixed Rotation",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInFixedRotation);
			this->vInFixedRotation->SetSubType(0,1,1.0,0.0,false,true,false);

			this->FHost->CreateValueInput("Is Bullet",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInIsBullet);
			this->vInIsBullet->SetSubType(0,1,1.0,0.0,false,true,false);

			this->FHost->CreateStringInput("Custom",TSliceMode::Dynamic,TPinVisibility::True,this->vInCustom);
			this->vInCustom->SetSubType("",false);

			this->FHost->CreateValueInput("TTL",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInTTL);
			this->vInTTL->SetSubType(Double::MinValue,Double::MaxValue,0.01,1.0,false,false,false);

			this->FHost->CreateValueInput("Has TTL",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInHasTTL);
			this->vInHasTTL->SetSubType(0,1,1.0,0.0,false,true,false);

			this->FHost->CreateValueInput("Do Create",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInDoCreate);
			this->vInDoCreate->SetSubType(0,1,1.0,0.0,true,false,false);

			this->FHost->CreateNodeOutput("Body",TSliceMode::Dynamic,TPinVisibility::True,this->vOutBodies);
			this->vOutBodies->SetSubType(ArrayUtils::SingleGuidArray(BodyDataType::GUID),BodyDataType::FriendlyName);
			this->vOutBodies->SetInterface(this->mBodies);

			this->FHost->CreateValueOutput("Body Id",1,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vOutId);
			this->vOutId->SetSubType(Double::MinValue,Double::MaxValue,1,0.0,false,false,true);

			
		}

		void Box2dCreateBodyNode::Configurate(IPluginConfig^ Input)
		{

		}

		void Box2dCreateBodyNode::Evaluate(int SpreadMax)
		{
			double dblcreate;
			this->vInDoCreate->GetValue(0,dblcreate);

			this->mBodies->Reset();
			List<int>^ bodyids = gcnew List<int>();

			int shapeidx = 0;

			if (dblcreate >= 0.5 && this->vInWorld->IsConnected && this->vInShapes->IsConnected) 
			{
				if (this->mWorld->GetIsValid()) 
				{
					double x,y,a,vx,vy,va,bull,fr,ld,ad,dblhasttl,dblttl;
					String^ cust;
					

					for (int i = 0; i < SpreadMax; i++) 
					{
						this->vInPosition->GetValue2D(i,x,y);
						this->vInVelocity->GetValue2D(i,vx,vy);
						this->vInLinearDamping->GetValue(i,ld);
						this->vInAngularDamping->GetValue(i,ad);
						this->vInAngularVelocity->GetValue(i,va);
						this->vInIsBullet->GetValue(i,bull);
						this->vInFixedRotation->GetValue(i,fr);
						this->vInCustom->GetString(i,cust);
						this->vInAngle->GetValue(i,a);
						this->vInTTL->GetValue(i,dblttl);
						this->vInHasTTL->GetValue(i,dblhasttl);


						b2BodyDef bodydef;
						bodydef.position.Set(Convert::ToSingle(x),Convert::ToSingle(y));
						bodydef.isBullet = (bull >= 0.5);
						bodydef.fixedRotation = (fr >= 0.5);
						bodydef.angle = Convert::ToSingle(a * (Math::PI * 2.0));
						bodydef.linearDamping = Convert::ToSingle(ld);
						bodydef.angularDamping = Convert::ToSingle(ad);

					
						BodyCustomData* bdata = new BodyCustomData();
						
						bdata->Id = this->mWorld->GetNewBodyId();
						bdata->Custom = (char*)(void*)Marshal::StringToHGlobalAnsi(cust);
						bdata->HasTTL = dblhasttl > 0.5;
						bdata->TTL = dblttl;
						

						bodyids->Add(bdata->Id);

						bool testcount;
						int vcount = 0;

						int realslice;
						this->vInShapes->GetUpsreamSlice(i % this->vInShapes->SliceCount,realslice);
						b2ShapeDef* shapedef = this->mShapes->GetSlice(realslice);

						if (shapedef->type == e_edgeShape)
						{
							b2EdgeChainDef* chain = (b2EdgeChainDef*)shapedef;
							vcount += chain->vertexCount;
							if (chain->isALoop)
							{
								vcount++;
							}
						}
						else
						{
							vcount++;
						}
						
						testcount = this->mWorld->GetWorld()->GetProxyCount() + vcount <= b2_maxProxies;
					
						if (testcount)
						{
							b2Body* body = this->mWorld->GetWorld()->CreateBody(&bodydef);
							body->SetLinearVelocity(b2Vec2(Convert::ToSingle(vx),Convert::ToSingle(vy)));
							body->SetAngularVelocity(Convert::ToSingle(va));
							body->SetUserData(bdata);

							float dens = 0.0f;

							int realslice;
							this->vInShapes->GetUpsreamSlice(i % this->vInShapes->SliceCount,realslice);
					
							b2ShapeDef* shapedef = this->mShapes->GetSlice(realslice);
							String^ shapecust = this->mShapes->GetCustom(realslice);

							dens += shapedef->density;


							b2Shape* shape = body->CreateShape(shapedef);
							
							if (shape->GetType() == e_edgeShape)
							{
								b2Shape* node = body->GetShapeList();
								while(node)
								{
									if (node->GetUserData() == nullptr)
									{
										ShapeCustomData* sdata = new ShapeCustomData();
										sdata->Id = this->mWorld->GetNewShapeId();
										sdata->Custom = (char*)(void*)Marshal::StringToHGlobalAnsi(shapecust);
										node->SetUserData(sdata);										
									}

									node = node->GetNext();
								}

							}
							else
							{
								ShapeCustomData* sdata = new ShapeCustomData();
								sdata->Id = this->mWorld->GetNewShapeId();
								sdata->Custom = (char*)(void*)Marshal::StringToHGlobalAnsi(shapecust);
								shape->SetUserData(sdata);
							}


							if (dens != 0.0) 
							{
								body->SetMassFromShapes();
							}

							//this->createdbodies->push_back(body);
							this->mBodies->Add(body);
						}

					}
				}

				
			} 

			this->vOutBodies->SliceCount = this->mBodies->Size();
			this->vOutBodies->MarkPinAsChanged();

			this->vOutId->SliceCount = bodyids->Count;
			for (int i = 0; i < bodyids->Count; i++)
			{
				this->vOutId->SetValue(i,bodyids[i]);
			}

		}


		void Box2dCreateBodyNode::ConnectPin(IPluginIO^ Pin)
		{
			//cache a reference to the upstream interface when the NodeInput pin is being connected
        	if (Pin == this->vInWorld)
        	{
				INodeIOBase^ usI;
				this->vInWorld->GetUpstreamInterface(usI);
				this->mWorld = (WorldDataType^)usI;
        	}
			if (Pin == this->vInShapes) 
			{
				INodeIOBase^ usI;
				this->vInShapes->GetUpstreamInterface(usI);
				this->mShapes = (ShapeDefDataType^)usI;
			}
		}


		void Box2dCreateBodyNode::DisconnectPin(IPluginIO^ Pin)
		{
			if (Pin == this->vInWorld)
        	{
        		this->mWorld = nullptr;
        	}
			if (Pin == this->vInShapes)
        	{
        		this->mShapes = nullptr;
        	}
		}
	}
}
