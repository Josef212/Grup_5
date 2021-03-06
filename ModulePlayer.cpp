#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

#define DELAY_TIME 200;


ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
	collider = NULL;
	current_animation = NULL;

	// idle animation (just the ship)
	idle.frames.PushBack({66, 1, 32, 14});

	// move upwards
	up.frames.PushBack({100, 1, 32, 14});
	up.loop = false;
	up.speed = 0.0f;

	upup.frames.PushBack({132, 0, 32, 14});
	upup.loop = false;
	upup.speed = 0.0f;
	
	// Move down
	down.frames.PushBack({33, 1, 32, 14});
	down.loop = false;
	down.speed = 0.0f;

	downdown.frames.PushBack({ 0, 1, 32, 14 });
	downdown.loop = false;
	downdown.speed = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	graphics = App->textures->Load("rtype/ship.png");

	position.x = 120;
	position.y = 100;

	speed = 1.0f;
	power_up = 0;
	charge = 0;
	exploding = false;
	charging = false;
	charging_animation = false;
	laser_tri = false;
	player_state = PLAYER_IDLE;
	Uint32 delay_time = 0;

	collider = App->collision->AddCollider({ position.x, position.y, 32, 12 }, COLLIDER_PLAYER, this);

	god_mode = false;

	App->particles->AddParticle(App->particles->bar, SCREEN_WIDTH / 2 - 262, SCREEN_HEIGHT + 5);


	App->particles->AddParticle(App->particles->beam, SCREEN_WIDTH / 2 - 140 - 98, SCREEN_HEIGHT);

	if (lifes > 0)
	{
		App->particles->AddParticle(App->particles->lifes, 3, SCREEN_HEIGHT + 3);
	}
	if (lifes > 1)
	{
		App->particles->AddParticle(App->particles->lifes, 19, SCREEN_HEIGHT + 3);
	}
	if (lifes > 2)
	{
		App->particles->AddParticle(App->particles->lifes, 35, SCREEN_HEIGHT + 3);
	}

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
	player_input = PLAYER_INPUT_IDLE;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (position.x > (App->renderer->camera.x) / -3)
			position.x -= 1.2*speed;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (position.x < (App->renderer->camera.x / -3) + SCREEN_WIDTH - 32)
			position.x += speed;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		if (position.y < (SCREEN_HEIGHT - 16))
		{
			position.y += speed;

			player_input = PLAYER_INPUT_DOWN;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		if (position.y > 0)
		{
			position.y -= speed;

			player_input = PLAYER_INPUT_UP;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && charging == false && exploding == false)
	{
		charge = SDL_GetTicks();
		charging = true;
	}

	if (charging == true && SDL_GetTicks() - charge > 400 && exploding == false)
	{
		if (charging_animation == false)
		{
			App->particles->AddParticle(App->particles->laser_charged_anim, position.x + 32, position.y - 9);
			charging_animation = true;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP && exploding == false)
	{
		if (SDL_GetTicks() - charge < 400)
		{
			App->particles->AddParticle(App->particles->laser_anim, position.x + 32, position.y);
			App->particles->AddParticle(App->particles->laser, position.x + 28, position.y + 5, COLLIDER_PLAYER_SHOT);
		}

		else if (SDL_GetTicks() - charge < 550)
		{
			App->particles->AddParticle(App->particles->laser_anim, position.x + 32, position.y);
			App->particles->AddParticle(App->particles->laser_charged1, position.x + 28, position.y + 1, COLLIDER_PLAYER_SHOT_CHARGED);
		}

		else if (SDL_GetTicks() - charge < 700)
		{
			App->particles->AddParticle(App->particles->laser_anim, position.x + 32, position.y);
			App->particles->AddParticle(App->particles->laser_charged2, position.x + 28, position.y + 1, COLLIDER_PLAYER_SHOT_CHARGED);
		}

		else if (SDL_GetTicks() - charge < 850)
		{
			App->particles->AddParticle(App->particles->laser_anim, position.x + 32, position.y);
			App->particles->AddParticle(App->particles->laser_charged3, position.x + 28, position.y, COLLIDER_PLAYER_SHOT_CHARGED);
		}

		else if (SDL_GetTicks() - charge < 1000)
		{
			App->particles->AddParticle(App->particles->laser_anim, position.x + 32, position.y);
			App->particles->AddParticle(App->particles->laser_charged4, position.x + 28, position.y, COLLIDER_PLAYER_SHOT_CHARGED);
		}

		else if (SDL_GetTicks() - charge > 1000)
		{
			App->particles->AddParticle(App->particles->laser_anim, position.x + 32, position.y);
			App->particles->AddParticle(App->particles->laser_charged5, position.x + 28, position.y - 1, COLLIDER_PLAYER_SHOT_CHARGED);
		}

		if (power_up == 1)
		{
			App->particles->AddParticle(App->particles->laser_powerup, position.x + 16, position.y - 15, COLLIDER_PLAYER_SHOT_POWERUP);
		}
		if (power_up == 2)
		{
			App->particles->AddParticle(App->particles->laser_blue, position.x + 32, position.y + 5, COLLIDER_PLAYER_SHOT);
			App->particles->AddParticle(App->particles->laser_blue2, position.x + 32, position.y - 2, COLLIDER_PLAYER_SHOT2);
			App->particles->AddParticle(App->particles->laser_blue3, position.x + 32, position.y - 2, COLLIDER_PLAYER_SHOT2);
			laser_tri = true;
		}
		charging = false;
		charging_animation = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		player_input = PLAYER_INPUT_IDLE;
	}

	//God Mode
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		god_mode = !god_mode;

	collider->rect.x = position.x;
	collider->rect.y = position.y;

	Fsm();

	// Draw everything --------------------------------------

	App->renderer->Blit(graphics, position.x, position.y, &(current_animation->GetCurrentFrame()));

	return UPDATE_CONTINUE;
}

// TODO 4: Detectar colisio del jugador y retornar a la pantalla de inici

void ModulePlayer::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->type == COLLIDER_POWER_UP || c2->type == COLLIDER_POWER_UP)
	{
		power_up++;
	}
	else if (exploding == false && god_mode == false)
	{
		lifes--;
		if (lifes > 0)
		{
			App->fade->FadeToBlack(App->scene_space, App->scene_continue, 3.0f);
		}
		else
		{
			App->fade->FadeToBlack(App->scene_space, App->scene_intro);
		}
		exploding = true;
		speed = 0;
		App->scene_space->player_speed = 0;
		App->enemy->shooting = false;
		CleanUp();
		App->particles->AddParticle(App->particles->explosion, position.x, position.y);
	}
}

void ModulePlayer::Fsm()
{
	if (delay_time > 0)
	{
		if (SDL_GetTicks() - delay_time > 200/*DELAY_TIME*/)
		{
			delay_time = 0;
		}
	}

	if (delay_time == 0)
	{
		switch (player_state)
			{
		case PLAYER_IDLE:
		{
							current_animation = &idle;

							switch (player_input)
							{
							case PLAYER_INPUT_IDLE:
								break;
							case PLAYER_INPUT_UP: player_state = PLAYER_UP; delay_time = SDL_GetTicks();
								break;
							case PLAYER_INPUT_DOWN: player_state = PLAYER_DOWN; delay_time = SDL_GetTicks();
								break;
							default:
								break;
							}
		}
			break;

		case PLAYER_UP:
		{
						  current_animation = &up;

						  switch (player_input)
						  {
						  case PLAYER_INPUT_IDLE: player_state = PLAYER_IDLE; delay_time = SDL_GetTicks();
							  break;
						  case PLAYER_INPUT_UP: player_state = PLAYER_UPUP; delay_time = SDL_GetTicks();
							  break;
						  case PLAYER_INPUT_DOWN: player_state = PLAYER_IDLE; delay_time = SDL_GetTicks();
							  break;
						  default:
							  break;
						  }
		}
			break;

		case PLAYER_UPUP:
		{
							current_animation = &upup;
							switch (player_input)
							{
							case PLAYER_INPUT_IDLE: player_state = PLAYER_UP; delay_time = SDL_GetTicks();
								break;
							case PLAYER_INPUT_UP: player_state = PLAYER_UPUP; delay_time = SDL_GetTicks();
								break;
							case PLAYER_INPUT_DOWN: player_state = PLAYER_UP; delay_time = SDL_GetTicks();
								break;
							default:
								break;
							}
		}
			break;

		case PLAYER_DOWN:
		{
							current_animation = &down;
							switch (player_input)
							{
							case PLAYER_INPUT_IDLE: player_state = PLAYER_IDLE; delay_time = SDL_GetTicks();
								break;
							case PLAYER_INPUT_UP: player_state = PLAYER_IDLE; delay_time = SDL_GetTicks();
								break;
							case PLAYER_INPUT_DOWN: player_state = PLAYER_DOWNDOWN; delay_time = SDL_GetTicks();
								break;
							default:
								break;
							}
		}
			break;

		case PLAYER_DOWNDOWN:
		{
								current_animation = &downdown;
								switch (player_input)
								{
								case PLAYER_INPUT_IDLE: player_state = PLAYER_DOWN; delay_time = SDL_GetTicks();
									break;
								case PLAYER_INPUT_UP: player_state = PLAYER_DOWN; delay_time = SDL_GetTicks();
									break;
								case PLAYER_INPUT_DOWN: player_state = PLAYER_DOWNDOWN; delay_time = SDL_GetTicks();
									break;
								default:
									break;
								}
		}
			break;

		}
	}
}