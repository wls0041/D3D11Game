#include "Framework.h"
#include "Physics.h"
#include "Physics/BulletPhysicsHelper.h"
#include "Physics/PhysicsDebugDraw.h"

#pragma warning(push, 0)
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#pragma warning(pop)

using namespace BulletPhysicsHelper;

static const int MAX_SOLVER_ITERATIONS	= 256;
static const float INTERNAL_FPS			= 60.0f;
static const Vector3 GRAVITY			= Vector3(0.0f, -9.81f, 0.0f);

Physics::Physics(Context * context)
	: ISubsystem(context)
	, max_sub_step(1)
	, is_simulating(false)
{
	timer	 = context->GetSubsystem<Timer>();
	renderer = context->GetSubsystem<Renderer>();
	SUBSCRIBE_TO_EVENT(EventType::Update, EVENT_HANDLER(Update));
}

Physics::~Physics()
{
}

const bool Physics::Initialize()
{
	//Create Physics Object
	broadphase				= new btDbvtBroadphase(); //���� �浹 ó�� �ܰ�� �Ѿ�� �� 'AABB'����� ���� �ʿ���� ������Ʈ�� ����
	collision_configuration = new btDefaultCollisionConfiguration(); //�������� �浹 ������ ���. ���� ������ �浹�� ����
	dispatcher				= new btCollisionDispatcher(collision_configuration); //���� ��� ������ ���� �浹 �˰����� Ư����Ŵ.
	constraint_solver		= new btSequentialImpulseConstraintSolver(); //��������. ������Ʈ �� ����� ������ ������� ����.
	world					= new btDiscreteDynamicsWorld(dispatcher, broadphase, constraint_solver, collision_configuration); //��� ������Ʈ�� ��� ������Ʈ�鰣�� �����ۿ��� ����ϴ� ����

	//Create Physics World
	world->setGravity(ToBtVector3(GRAVITY)); 
	world->getDispatchInfo().m_useContinuous = true; 
	/*
		DCD(Discrete Collision Detection)
		- �� �����Ӹ��� �浹�� üũ. object�� �۰ų� ������ �浹�� ������ �� ����(�̸� '�ͳθ�'�̶�� ��)

		CCD(Continuous Collision Detection) 
		- ������Ʈ�� ���ʷ� �浹�ϴ� ������ ����ؼ� �� ��Ȯ�� �浹 �˻縦 �� �� �ְ� ��. �ڵ�� �� ����� ����ϰڴٴ� ��.
	*/
	world->getSolverInfo().m_splitImpulse	 = false; 
	/*
		��ü�� �浹�� �� ����� ������ �ӵ��� Ȱ��/��Ȱ��. 
		��� �� ����� ������ �ٿ� �� �������� �� ��Ȯ�ϰ� ������Ű�� ����� ��?
		�浹 �� ��ü�� �и��Ǵ� ���� �����ϰ� �� ���� ����� �����Ͻø� 
		�ùķ��̼��� �������� ����߸��ٰ� ��.(��ü�� ����������)
	
	*/
	world->getSolverInfo().m_numIterations	 = MAX_SOLVER_ITERATIONS; //���� ��Ȯ�� ����� ���� ����� �ݺ��ϴ� Ƚ��. 256�� ������ �� ã��. 

	//Get Version
	const auto major = std::to_string(btGetVersion() / 100); //�̰� ���� �ڵ尡 ��� �� �̴� ���� �𸣰���.
	const auto minor = std::to_string(btGetVersion()).erase(0, 1); //0��°���� 1���� ���ڸ� ����. ���������� ������ �𸣰���.

	debug_draw = new PhysicsDebugDraw(context);
	world->setDebugDrawer(debug_draw);

	return true;
}

auto Physics::GetGravity() const -> const Vector3
{
	return Vector3();
}

void Physics::Update()
{
}
