#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
	current_animation = NULL;

	position.x = 100;
	position.y = 150;
	
	still.frames.PushBack({ 167, 1, 32, 16 });


	//up animation
	up.frames.PushBack({ 200, 1, 32, 16 });
	up.frames.PushBack({ 232, 1, 32, 16 });
	up.loop = false;
	up.speed = 0.5f;

	//down animation
	down.frames.PushBack({ 133, 1, 32, 16 });
	down.frames.PushBack({ 100, 1, 32, 16 });
	down.loop = false;
	down.speed = 0.5f;

	//Explosion_Player
	explosion.frames.PushBack({ 1, 343, 32, 28 });
	explosion.frames.PushBack({ 35, 343, 32, 28 });
	explosion.frames.PushBack({ 69, 343, 32, 28 });
	explosion.frames.PushBack({ 103, 343, 32, 28 });
	explosion.frames.PushBack({ 137, 343, 32, 28 });
	explosion.frames.PushBack({ 171, 343, 32, 28 });
	explosion.frames.PushBack({ 205, 343, 32, 28 });
	explosion.frames.PushBack({ 239, 343, 32, 28 });
	explosion.loop = false;
	explosion.speed = 1.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	graphics = App->textures->Load("ShipSprites_Alpha.png"); // arcade version

	position.x = 150;
	position.y = 120;

	speed = 1;

	// TODO 2: Afegir collider al jugador
	collider = App->collision->AddCollider({ position.x, position.y+2, 32, 13 }, COLLIDER_PLAYER, this);

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->textures->Unload(graphics);

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	collider->rect.x = position.x;
	collider->rect.y = position.y+2;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (position.x > (App->renderer->camera.x)/-3)
			position.x -= 1.2*speed;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (position.x < (App->renderer->camera.x / -3)+ SCREEN_WIDTH - 32)
			position.x += speed;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		if (position.y < SCREEN_HEIGHT - 16)
		{
			position.y += speed;

			if (current_animation != &down)
			{
				down.Reset();
				current_animation = &down;
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		if (position.y > 0)
		{
			position.y -= speed;
			if (current_animation != &up)
			{
				up.Reset();
				current_animation = &up;
			}
		}
	}


	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE)
		current_animation = &still;

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		App->particles->AddParticle(App->particles->laser, position.x + 28, position.y, COLLIDER_PLAYER_SHOT);
	}

	// Draw everything --------------------------------------

	App->renderer->Blit(graphics, position.x, position.y, &(current_animation->GetCurrentFrame()));

	return UPDATE_CONTINUE;
}

// TODO 4: Detectar colisio del jugador y retornar a la pantalla de inici// Fet

void ModulePlayer::OnCollision(Collider* c1, Collider* c2)
{
	if (has_collided == false)
	{
		has_collided = true;
		speed = 0;
		App->scene_stage1->player_speed = 0;
		current_animation = &explosion;
		while (current_animation == &explosion) 
		{
			App->renderer->Blit(graphics, position.x, position.y, &(current_animation->GetCurrentFrame()));
			if (App->player->explosion.Finished() == true)
			{
				current_animation = NULL;
			}
		}
		App->fade->FadeToBlack(App->scene_stage1, App->scene_title, 3.0f);
		
	}
}
