#include "Globals.h"
#include "Application.h"
#include "ModuleEnemy.h"
#include <math.h>

ModuleEnemy::ModuleEnemy(Application* app, bool start_enabled) : Module(app, start_enabled), graphics(NULL)
{}

ModuleEnemy::~ModuleEnemy()
{}

bool ModuleEnemy::Start()
{
	LOG("Loading enemies");
	graphics = App->textures->Load("rtype/RedEnemySprites.png");

	red.anim.frames.PushBack({ 5, 6, 21, 24 });
	red.anim.loop = true;
	red.anim.speed = 0.0f;
	red.alive = true;

	return true;
}

bool ModuleEnemy::CleanUp()
{
	LOG("Unloading enemies");
	App->textures->Unload(graphics);

	//TODO ClearAll

	return true;
}

update_status ModuleEnemy::PreUpdate()
{
	p2List_item<Enemy*>* tmp = EnemyList.getFirst();
	p2List_item<Enemy*>* tmp_next;
	while (tmp != NULL)
	{
		tmp_next = tmp->next;

		if (tmp->data->position.x < (App->renderer->camera.x / -3) + SCREEN_WIDTH - 32)
		{
			tmp->data->on_screen = true;
		}

		if (tmp->data->alive == false)
		{
			EnemyList.del(tmp);
		}


		tmp = tmp_next;
	}
	return UPDATE_CONTINUE;
}

update_status ModuleEnemy::Update()
{
	p2List_item<Enemy*>* tmp = EnemyList.getFirst();

	while (tmp != NULL)
	{
		Enemy* e = tmp->data;

		if (e->on_screen)
		{
			char x = 'sin';
			if (e->mov_type == x)
			{
				float t = SDL_GetTicks();
				e->position.y = 100 + 40 * sin(25 + t / 250);
				e->position.x--;
			}
		}

		if (tmp->data->alive)
		{
			e->collider->rect.x = e->position.x;
			e->collider->rect.y = e->position.y;

			App->renderer->Blit(graphics, e->position.x, e->position.y, &(e->anim.GetCurrentFrame()));
		}

		tmp = tmp->next;
	}
	return UPDATE_CONTINUE;
}

void ModuleEnemy::OnCollision(Collider* c1, Collider* c2)
{
	p2List_item<Enemy*>* tmp = EnemyList.getFirst();

	while (tmp != NULL)
	{
		Enemy* e = tmp->data;
		if (e->on_screen)
		{
			if (c1->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_PLAYER_SHOT)
			{
				e->alive = false;
				e->collider->to_delete = true;
				App->particles->AddParticle(App->particles->enemy_death, e->position.x, e->position.y);
			}
		}
		tmp = tmp->next;
	}


}

void ModuleEnemy::AddEnemy(const Enemy& enemy, int x, int y, char mov)
{
	Enemy* e = new Enemy(enemy);
	e->position.x = x;
	e->position.y = y;

	e->collider = App->collision->AddCollider({ e->position.x, e->position.y, 21, 24 }, COLLIDER_ENEMY, this);

	e->mov_type = mov;

	EnemyList.add(e);
}

Enemy::Enemy() :collider(NULL), alive(true), on_screen(false)
{}

Enemy::~Enemy()
{
	if (collider)
	{
		collider->to_delete = true;
	}
}
