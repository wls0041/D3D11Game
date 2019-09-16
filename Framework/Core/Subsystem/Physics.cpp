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

	//Get  - ��������� copy right ������ �ʿ��� �� ǥ���� ������ �ִٸ� �̷��� �̾� ���� ��. 
	const auto major = std::to_string(btGetVersion() / 100); //�̰� ���� �ڵ尡 ��� �� �̴� ���� �𸣰���.
	const auto minor = std::to_string(btGetVersion()).erase(0, 1); //0��°���� 1���� ���ڸ� ����. ���������� ������ �𸣰���.

	debug_draw = new PhysicsDebugDraw(context);
	world->setDebugDrawer(debug_draw);

	return true;
}

auto Physics::GetGravity() const -> const Vector3
{
	if (!world) {
		LOG_ERROR("Physics world is nullptr");
		return Vector3::Zero;
	}

	auto gravity = world->getGravity();
	if (!gravity) {
		LOG_ERROR("Unable To Get Gravity, ensure physics are properly initialized");
		return Vector3::Zero;
	}

	return ToVector3(gravity);
}

void Physics::Update()
{
	if (!world) return;
	if (!Engine::IsFlagsEnabled(EngineFlags::EngineFlags_Physics) || !Engine::IsFlagsEnabled(EngineFlags::EngineFlags_Game)) return;

	auto delta_time = timer->GetDeltaTimeSec();
	auto internal_time_step = 1.0f / INTERNAL_FPS;
	auto max_substeps = static_cast<int>(delta_time * INTERNAL_FPS);

	if (max_sub_step < 0) { //����max_substeps�ƴ�
		internal_time_step = delta_time;
		max_substeps = 1;
	}
	else if (max_sub_step > 0) max_substeps = Math::Max(max_substeps, max_sub_step); //���� ����ϴ� ���� �ð��� ���� ó��
	
	world->stepSimulation(delta_time, max_substeps, internal_time_step);
}
