#pragma once
#include <Component/SimpleScene.h>
#include "LabCamera.h"

class Laborator5 : public SimpleScene
{
	public:
		Laborator5();
		~Laborator5();

		void Init() override;

		struct Player {

			Player() : x(0), y(0), lifes(3), angle(0), typeShooting(1), scalePlayer(0.15f), 
				angleWhenDead(angle), disappear(false) {}
			Player(float x, float y, float angle, int lifes, int typeShooting, float scalePlayer, float angleWhenDead, bool disappear)
				: x(x), y(y), lifes(lifes), angle(angle), typeShooting(typeShooting), scalePlayer(scalePlayer), 
				angleWhenDead(angleWhenDead), disappear(disappear) {}
			//coordonatele player-ului
			float x;
			float y;
			int lifes;
			float angle;
			int typeShooting;
			float scalePlayer;
			float angleWhenDead;
			bool disappear;
			float scaleMinimapPlayer = 1.0f;
			bool gameOver = false;
		};

		struct Wolf {

			Wolf() : x(0), y(0), z(0), angle(0), lifes(10), angleWhenDead(angle), disappear(false), isOnDiagonal(false) {}
			Wolf(float x, float y, float z, float angle, int lifes, bool finishedDiag, float angleWhenDead, bool disappear, bool isOnDiagonal)
				: x(x), y(y), z(z), angle(angle), lifes(lifes), angleWhenDead(angleWhenDead), disappear(disappear), isOnDiagonal(isOnDiagonal){}
			float x;
			float y;
			float z;
			float angle;
			int lifes;
			float angleWhenDead;
			bool disappear;
			bool isOnDiagonal;
		};

		struct Pistol {

			Pistol() : x(0), y(0), z(0), angle(0), speed(0.05f), distancePistol(200), scalePistol(0.05f) {}
			Pistol(float x, float y, float angle, float z, float speed, int distancePistol, float scalePistol)
				: x(x), y(y), angle(angle), z(z), speed(speed), distancePistol(distancePistol), scalePistol(scalePistol) {}
			float x;
			float y;
			float z;
			float angle;
			float speed;
			int distancePistol;
			float scalePistol;
		};

		struct Rifle {

			Rifle() : x(0), y(0), z(0), angle(0), speed(0.6f), distanceRifle(200), scaleRifle(0.35f) {}
			Rifle(float x, float y, float z, float angle, float speed, int distanceRifle, float scaleRifle)
				: x(x), y(y), z(z), angle(angle), speed(speed), distanceRifle(distanceRifle), scaleRifle(scaleRifle) {}
			float x;
			float y;
			float z;
			float angle;
			float speed;
			int distanceRifle;
			float scaleRifle;
		};

		struct Grenade {

			Grenade() : x(0), y(0), z(0), angle(0), speed(0.05f), distanceGrenade(280), scaleGrenade(0.25f) {}
			Grenade(float x, float y, float z, float angle, float speed, int distanceGrenade, float scaleGrenade)
				: x(x), y(y), z(z), angle(angle), speed(speed), distanceGrenade(distanceGrenade), scaleGrenade(scaleGrenade) {}
			float x;
			float y;
			float z;
			float angle;
			float speed;
			int distanceGrenade;
			float scaleGrenade;
		};

		struct Lives {

			Lives() : x(0), y(0), z(0), angle(0) {}
			Lives(float x, float y, float z, float angle)
				:x(x), y(y), z(z), angle(angle) {}
			float x;
			float y;
			float z;
			float angle;
		};

		struct TowerBullet {

			TowerBullet() : x(0), y(0), z(0), speed(0.02f), distanceTowerBullet(200), lifesToDistroy(4) {}
			TowerBullet(float x, float y, float z, float  angle, float speed, float tBulletScale, float distanceTowerBullet)
				: x(x), y(y), z(z), angle(angle), speed(speed), tBulletScale(tBulletScale), distanceTowerBullet(distanceTowerBullet) {}
			float x;
			float y;
			float z;
			float angle;
			float speed;
			float tBulletScale = 0.05;
			float distanceTowerBullet;
			int lifesToDistroy;
			float scaleTowerBullets = 0.5f;
		};

		struct Tower {

			Tower() : x(0), y(0), z(0), angle(0), time(2) {}
			Tower(float x, float y, float z, float angle, float time)
				:x(x), y(y), z(z), time(time) {}
			float x;
			float y;
			float z;
			float angle;
			float time;
			std::vector<TowerBullet> towerBulletsVector;
		};

		

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix) override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		void DrawScene();			//pentru a desena obiectele din scena
		void PistolShooting();		//pentru a trage cu pistolul
		void RifleShooting();		//pentru a trage cu pusca
		void GrenadeShooting();		//pentru a trage cu grenade
		void PistolWolvesCollision();		//pentru coliziunea dinntre gloantele pistolului si lupi
		void RifleWolvesCollision();		//pentru coliziunea dintre gloantele pustii si lupi
		void GrenadeWolvesCollision();		//pentru coliziunea dintre grenade si lupi
		void TowerShooting();
		void RenderMeshMinimap(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix);	//pentru randarea elementelor din minimap
		void DrawMinimap();
		void TowersWolvesCollision();

	protected:
		Laborator::Camera *camera;
		Laborator::Camera *minimapCamera;
		glm::mat4 projectionMatrix;
		glm::mat4 minimapMatrix;

		bool renderCameraTarget;
		int fov;
		float var;

		//ADAUGATE DE MINE PT MISCAREA PLAYER-ULUI
		Player player;
		float playerSpeed = 2;
		//vectori pentru tipurile de pusti
		std::vector<Pistol> pistolVector;
		std::vector<Rifle> rifleVector;
		std::vector<Grenade> grenadeVector;
		std::vector<Lives> lifesVector;		//vector pentru vietile player-ului
		std::vector<Wolf> wolvesVector;		//vector pentru inamici
		std::vector<Tower> towersVector;	//vector pentru turnuri
		std::vector<TowerBullet> towerBulletsVector;	//vector pentru gloantele date de turnuri

		int count = 0;				//folosit pentru miscorarea timpului de aparitie a inamicilor
		float nextTime = 50;		//timpul dupa care incep inamicii sa apara la 3 secunde
		float auxTime = 2;			//folosit pentru ca inamicii sa apara la 5, respectiv 3 secunde
		float angularStep = 0;
		float collisionDistPistol = 0;
		float collisionDistRifle = 0;
		float collisionDistGrenade = 0;
		float collisionDistTower = 0;
		float deltaDistance = 1;
		float deltaDistanceTower = 0.4f;
		int perspective = 1;		//pentru perspectiva in care se afla player-ul cand trage cu gloante
		double auxTowerTime = 2;
};
