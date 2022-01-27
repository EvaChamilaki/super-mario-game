#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__
#include "Animation.h"
typedef uint64_t timestamp_t;

enum animatorstate_t {
	ANIMATOR_FINISHED = 0,
	ANIMATOR_RUNNING = 1,
	ANIMATOR_STOPPED = 2
};

class Animator {
public:
	using OnFinish = std::function<void(Animator*)>;
	using OnStart = std::function<void(Animator*)>;
	using OnAction = std::function<void(Animator*, const Animation&)>;

protected:
	timestamp_t		lastTime = 0;
	animatorstate_t state = ANIMATOR_FINISHED;
	OnFinish		onFinish;
	OnStart			onStart;
	OnAction		onAction;
	void			NotifyStopped(void);
	void			NotifyStarted(void);
	void			NotifyAction(void);
	void			Finish(bool isForced = false);

public:
	void			Stop(void);
	bool			HasFinished(void) { return state != ANIMATOR_RUNNING; }
	virtual void	TimeShift(timestamp_t offset);
	virtual void	Progress(timestamp_t currTime) = 0;
	template <typename Tfunc> void SetOnFinish(const Tfunc& f) { onFinish = f; }
	template <typename Tfunc> void SetOnStart(const Tfunc& f) { onStart = f; }
	template <typename Tfunc> void SetOnAction(const Tfunc& f) { onAction = f; }
};

Animator(void);
Animator(const Animator&) = delete;
Animator(Animator&&) = delete;

void Animator::Finish(bool isForced) {
	if (!HasFinished()) {
		state = isForced ? ANIMATOR_STOPPED : ANIMATOR_FINISHED;
		NotifyStopped();
	}
}

void Animator::Stop(void) {
	Finish(true);
}

void Animator::NotifyStopped(void) {
	if (onFinish)
		(onFinish)(this);
}

void Animator::NotifyAction(const Animation& anim) {
	if (onAction)
		(onAction)(this, anim);
}

void Animator::TimeShift(timestamp_t offset) {
	lastTime += offset;
}

class MovingAnimator : public Animator {
protected:
	MovingAnimation* anim = nullptr;
	unsigned		 currRep = 0;

public:
	void			 Progress(timestamp_t currTime);
	auto			 GetAnim(void) const ->const MovingAnimation& { return *anim; }
	void			 Start(MovingAnimation* a, timestamp_t t)
	{
		anim = a;
		lastTime = t;
		state = ANIMATOR_RUNNING;
		currRep = 0;
		NotifyStarted();
	}

	MovingAnimator(void) = default;
}

void MovingAnimator::Progress(timestamp_t currTime) {
	while (currTime > lastTime && (currTime - lastTime) >= anim->GetDelay) {
		lastTime += anim->GetDelay();
		NotifyAction(*anim);
		if (!anim->IsForever() && ++currRep == anim->GetReps()) {
			state = ANIMATOR_FINISHED;
			NotifyStopped();
			return;
		}
	}
}

void Sprite_MoveAction(Sprite* sprite, const MovingAnimation& anim) {
	sprite->Move(anim.GetDx(), anim.GetDy());
}

animator->SetOnAction(
	[sprite](Animator* animator, const Animation& anim) {
		assert(dynamic_cast<const MovingAnimation*>(&anim));
		Sprite_MoveAction(
			sprite,
			(const MovingAnimation&)anim
		);
	}
);

class FrameRangeAnimator : public Animator {
protected:
	FrameRangeAnimation*	anim = nullptr;
	unsigned				currFrame = 0; // animation state
	unsigned				currRep = 0; // animation state
public:
	void					Progress(timestamp_t currTime);
	unsigned				GetCurrFrame(void) const { return currFrame; }
	unsigned				GetCurrRep(void) const { return currRep; }
	void					Start(FrameRangeAnimation* a, timestamp_t t) {
		anim = a;
		lastTime = t;
		state = ANIMATOR_RUNNING;
		currFrame = anim->GetStartFrame();
		currRep = 0;
		NotifyStarted();
		NotifyAction(*anim);
	}
	FrameRangeAnimator(void) = default;
};

void FrameRangeAnimator::Progress(timestamp_t currTime) {
	while (currTime > lastTime && (currTime - lastTime) >= anim->GetDelay()) {
		if (currFrame == anim->GetEndFrame()) {
			assert(anim->IsForever() || currRep < anim->GetReps());
			currFrame = anim->GetStartFrame(); // flip to start
		}
		else
			++currFrame;
		lastTime += anim->GetDelay();
		NotifyAction(*anim);
		if (currFrame == anim->GetEndFrame())
			if (!anim->IsForever() && ++currRep == anim->GetReps()) {
				state = ANIMATOR_FINISHED;
				NotifyStopped();
				return;
			}
	}
}

void FrameRange_Action(Sprite* sprite, Animator* animator, const FrameRangeAnimation& anim) {
	auto* frameRangeAnimator = (FrameRangeAnimator*)animator;
	if (frameRangeAnimator->GetCurrFrame() != anim.GetStartFrame() ||
		frameRangeAnimator->GetCurrRep())
		sprite->Move(anim.GetDx(), anim.GetDy());
	sprite->SetFrame(frameRangeAnimator->GetCurrFrame());
}
animator->SetOnAction(
	[sprite](Animator* animator, const Animation& anim) {
		FrameRange_Action(sprite, animator, (const FrameRangeAnimation&)anim);
	}
);


#endif