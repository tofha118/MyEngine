#pragma once
class Timer
{
	DECLARE_SINGLE(Timer);

public:
	void Init();
	void Update();

	uint32 GetFps() { return _fps; }  //평균적으로 몇프레임 찍혔는지 갯인해줄거임
	float GetDeltaTime() { return _deltaTime; }  //이전프레임까지 경과된 시간

private:
	uint64	_frequency = 0;
	uint64	_prevCount = 0;
	float	_deltaTime = 0.f;

private:
	//프레임을 계산하기 위함
	uint32	_frameCount = 0;
	float	_frameTime = 0.f;
	uint32	_fps = 0;

};

