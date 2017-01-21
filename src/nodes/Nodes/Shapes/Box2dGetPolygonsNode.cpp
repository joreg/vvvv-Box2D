
#include "Box2dGetPolygonsNode.h"
#include "../../Internals/Data/ShapeCustomData.h"
#include "../../Internals/Data/BodyCustomData.h"

using namespace System::Collections::Generic;
using namespace VVVV::Utils::VMath;

namespace VVVV 
{
	namespace Nodes 
	{
		Box2dGetPolygonsNode::Box2dGetPolygonsNode(void)
		{
		}

		void Box2dGetPolygonsNode::SetPluginHost(v4::IPluginHost^ Host) 
		{
			this->FHost = Host;

			this->FHost->CreateNodeInput("Shapes",v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInShapes);
			this->vInShapes->SetSubType(VVVV::Utils::ArrayUtils::SingleGuidArray(v4b2d::ShapeDataType::GUID), v4b2d::ShapeDataType::FriendlyName);

			this->FHost->CreateValueInput("Closed Polygons",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInClosed);
			this->vInClosed->SetSubType(0,1,1,0,false,true,false);

			this->FHost->CreateValueInput("Local Coordinates",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInLocal);
			this->vInLocal->SetSubType(0,1,1,0,false,true,false);


			this->FHost->CreateValueOutput("Centers",2,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutCenters);
			this->vOutCenters->SetSubType2D(System::Double::MinValue,System::Double::MaxValue,0.01,0.0,0.0,false,false,false);

			this->FHost->CreateValueOutput("Vertices",2,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutVertices);
			this->vOutVertices->SetSubType2D(System::Double::MinValue,System::Double::MaxValue,0.01,0.0,0.0,false,false,false);

			this->FHost->CreateValueOutput("Vertices Count",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutVerticesCount);
			this->vOutVerticesCount->SetSubType(System::Double::MinValue,System::Double::MaxValue,0.01,0.0,false,false,true);

			this->FHost->CreateValueOutput("Is Sensor",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutIsSensor);
			this->vOutIsSensor->SetSubType(System::Double::MinValue,System::Double::MaxValue,0.01,0.0,false,true,false);

			this->FHost->CreateStringOutput("Custom",v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutCustom);
			this->vOutCustom->SetSubType("",false);

			this->FHost->CreateValueOutput("Shape Id",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutId);
			this->vOutId->SetSubType(System::Double::MinValue,System::Double::MaxValue,1,0.0,false,false,true);

			this->FHost->CreateValueOutput("Body Id",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutBodyId);
			this->vOutBodyId->SetSubType(System::Double::MinValue,System::Double::MaxValue,1,0.0,false,false,true);

			this->FHost->CreateValueOutput("LifeTime",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutLifeTime);
			this->vOutLifeTime->SetSubType(System::Double::MinValue,System::Double::MaxValue,1,0.0,false,false,false);
		}


		void Box2dGetPolygonsNode::Configurate(v4::IPluginConfig^ Input)
		{

		}

		
		void Box2dGetPolygonsNode::Evaluate(int SpreadMax)
		{
			if (this->vInClosed->PinIsChanged || this->vInLocal->PinIsChanged)
			{
				double dblclosed,dbllocal;
				this->vInClosed->GetValue(0, dblclosed);
				this->vInLocal->GetValue(0,dbllocal);

				this->m_closed  = dblclosed >= 0.5;
				this->m_local = dbllocal >= 0.5;
			}

			if (this->vInShapes->IsConnected) 
			{
				std::vector<b2Vec2> centers;
				std::vector<b2Vec2> vertices;
				std::vector<int> vcount;
				std::vector<int> ids;
				std::vector<bool> issensor;
				gen::List<System::String^>^ custs = gcnew gen::List<System::String^>(); 
				gen::List<int>^ bodyids = gcnew gen::List<int>();
				gen::List<double>^ lifetime = gcnew gen::List<double>();

				int cnt = 0;
				for (int i = 0; i < this->vInShapes->SliceCount ; i++) 
				{
					int realslice;
					this->vInShapes->GetUpsreamSlice(i,realslice);
					b2Shape* shape = this->m_polygons->GetSlice(realslice);
					
					if (shape->GetType() == e_polygonShape) 
					{
						b2PolygonShape* poly = (b2PolygonShape*)shape;
						if (poly->GetVertexCount() > 0) 
						{
							if (this->m_closed)
							{
								vcount.push_back(poly->GetVertexCount() + 1);
							}
							else
							{
								vcount.push_back(poly->GetVertexCount());
							}

							centers.push_back(poly->GetBody()->GetWorldPoint(poly->GetCentroid()));

							const b2Vec2* verts = poly->GetVertices();
							for (int j=0; j < poly->GetVertexCount();j++) 
							{
								if (this->m_local)
								{
									vertices.push_back(verts[j]);
								}
								else
								{
									vertices.push_back(poly->GetBody()->GetWorldPoint(verts[j]));
								}
							}

							if (this->m_closed)
							{
								if (this->m_local)
								{
									vertices.push_back(verts[0]);
								}
								else
								{
									vertices.push_back(poly->GetBody()->GetWorldPoint(verts[0]));
								}
							}

							ShapeCustomData* sdata = (ShapeCustomData*)shape->GetUserData();
							ids.push_back(sdata->Id);
							issensor.push_back(shape->IsSensor());
							System::String^ str = gcnew System::String(sdata->Custom);
							custs->Add(str);
							lifetime->Add(sdata->LifeTime);

							BodyCustomData* bdata = (BodyCustomData*)shape->GetBody()->GetUserData();
							bodyids->Add(bdata->Id);

							cnt++;
						}
					}
				}

				this->vOutVertices->SliceCount = vertices.size();
				this->vOutVerticesCount->SliceCount = vcount.size();
				this->vOutCenters->SliceCount = centers.size();
				this->vOutId->SliceCount = ids.size();
				this->vOutIsSensor->SliceCount = issensor.size();
				this->vOutCustom->SliceCount = ids.size();
				this->vOutBodyId->SliceCount = ids.size();
				this->vOutLifeTime->SliceCount = ids.size();

				for (int i = 0; i <  vertices.size() ; i++) 
				{
					this->vOutVertices->SetValue2D(i,vertices.at(i).x,vertices.at(i).y);
				}

				for (int i = 0; i < vcount.size() ; i++) 
				{
					this->vOutVerticesCount->SetValue(i,vcount.at(i));
					this->vOutCenters->SetValue2D(i,centers.at(i).x,centers.at(i).y);
					this->vOutId->SetValue(i,ids.at(i));
					this->vOutIsSensor->SetValue(i, issensor.at(i));
					this->vOutCustom->SetString(i, custs[i]);
					this->vOutBodyId->SetValue(i, bodyids[i]);
					this->vOutLifeTime->SetValue(i, lifetime[i]);
				}
			}
		}

		void Box2dGetPolygonsNode::ConnectPin(v4::IPluginIO^ Pin)
		{
			if (Pin == this->vInShapes) 
			{
				v4::INodeIOBase^ usI;
				this->vInShapes->GetUpstreamInterface(usI);
				this->m_polygons = (v4b2d::ShapeDataType^)usI;
			}
		}

		void Box2dGetPolygonsNode::DisconnectPin(v4::IPluginIO^ Pin)
		{
			if (Pin == this->vInShapes)
        	{
        		this->m_polygons = nullptr;
        	}
		}
	}
}


