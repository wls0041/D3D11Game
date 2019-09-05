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
	broadphase				= new btDbvtBroadphase(); //상세한 충돌 처리 단계로 넘어가기 전 'AABB'계산을 통해 필요없는 오브젝트를 제거
	collision_configuration = new btDefaultCollisionConfiguration(); //광범위한 충돌 감지를 담당. 보다 정밀한 충돌을 감지
	dispatcher				= new btCollisionDispatcher(collision_configuration); //여러 모양 유형에 대해 충돌 알고리듬을 특정시킴.
	constraint_solver		= new btSequentialImpulseConstraintSolver(); //제약조건. 오브젝트 간 연결된 부위의 운동범위를 제한.
	world					= new btDiscreteDynamicsWorld(dispatcher, broadphase, constraint_solver, collision_configuration); //모든 오브젝트를 담고 오브젝트들간의 물리작용을 계산하는 공간

	//Create Physics World
	world->setGravity(ToBtVector3(GRAVITY)); 
	world->getDispatchInfo().m_useContinuous = true; 
	/*
		DCD(Discrete Collision Detection)
		- 매 프레임마다 충돌을 체크. object가 작거나 빠르면 충돌을 무시할 수 있음(이를 '터널링'이라고 함)

		CCD(Continuous Collision Detection) 
		- 오브젝트가 최초로 충돌하는 지점을 계산해서 더 정확한 충돌 검사를 할 수 있게 함. 코드는 이 방식을 사용하겠다는 것.
	*/
	world->getSolverInfo().m_splitImpulse	 = false; 
	/*
		물체가 충돌할 때 생기는 별도의 속도를 활성/비활성. 
		계산 상 생기는 오차를 줄여 원 에너지를 더 정확하게 보존시키는 기능인 듯?
		충돌 시 객체가 분리되는 힘을 제한하고 더 나은 결과를 생성하시만 
		시뮬레이션의 안정성을 떨어뜨린다고 함.(객체가 많아질수록)
	
	*/
	world->getSolverInfo().m_numIterations	 = MAX_SOLVER_ITERATIONS; //보다 정확한 결과를 위해 계산을 반복하는 횟수. 256인 이유는 못 찾음. 

	//Get Version
	const auto major = std::to_string(btGetVersion() / 100); //이걸 쓰는 코드가 없어서 왜 뽑는 건지 모르겠음.
	const auto minor = std::to_string(btGetVersion()).erase(0, 1); //0번째부터 1개의 문자를 지움. 마찬가지로 이유는 모르겠음.

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
