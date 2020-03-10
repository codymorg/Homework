//I take care of rendering things!

#include "RoboMath.h"
#include <chrono>


class Line
{
public:
  Line(Vector2 start, Vector2 end, Vector3 color, float TTL);

  void update();
  void draw();
  bool isValid = true;

private:
  Vector2 start_;
  Vector2 end_;
  Vector3 color_;
  float TTL_;
  std::chrono::time_point<std::chrono::steady_clock> beginTime_;

};

class RenderManager
{
public:
	static void StaticInit();
	static std::unique_ptr< RenderManager >	sInstance;

	void Render();
	void RenderComponents();
  void RenderLines();

	//vert inefficient method of tracking scene graph...
	void AddComponent( SpriteComponent* inComponent );
	void RemoveComponent( SpriteComponent* inComponent );
  void AddLine(Vector2 start, Vector2 end, Vector3 color, float TTL = 2.0f);
	int	 GetComponentIndex( SpriteComponent* inComponent ) const;

	SDL_Rect						mViewTransform;
private:

	RenderManager();

	//this can't be only place that holds on to component- it has to live inside a GameObject in the world
	vector< SpriteComponent* >		mComponents;
  vector<Line> lines;



};

