//I take care of rendering things!

#include "RoboMath.h"
#include <chrono>


class Line
{
public:
  Line(Vector2 start, Vector2 end, Vector3 color, float TTL, int ID);

  void update();
  void draw();
  bool intersect(Vector2 center, float radius);

  bool isValid = true;
  Vector3 color;
  int networkID;

  Vector2 start_;
  Vector2 end_;
private:
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
  std::pair<Vector2,Vector2> GetHyperYarnLine();

	//vert inefficient method of tracking scene graph...
	void AddComponent( SpriteComponent* inComponent );
	void RemoveComponent( SpriteComponent* inComponent );
  void AddLine(Vector2 start, Vector2 end, Vector3 color, float TTL, int ID);
  void AddEnemyLine(Vector2 start, Vector2 end, Vector3 color, float TTL);
	int	 GetComponentIndex( SpriteComponent* inComponent ) const;

	//this can't be only place that holds on to component- it has to live inside a GameObject in the world
	SDL_Rect						mViewTransform;
	vector< SpriteComponent* >		mComponents;
  bool hyperYarnActive = false;
private:

	RenderManager();

  vector<Line> lines;



};

