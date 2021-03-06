#include "stdafx.h"
#include "battle.h"

HRESULT battle::init()
{
	// UI 이미지 등록
	_bg = IMAGEMANAGER->findImage("BattleBg");
	_tetrisBoard = IMAGEMANAGER->findImage("TetrisBoard");
	_fieldGlass = IMAGEMANAGER->findImage("FieldGlass");
	_room = IMAGEMANAGER->findImage("BattleRoom");
	_nextBlock = IMAGEMANAGER->findImage("NextBlock");
	_holdBlock = IMAGEMANAGER->findImage("HoldBlock");
	_screenOut = IMAGEMANAGER->findImage("ScreenOut");
	_activeSlot = IMAGEMANAGER->findImage("ActiveSlot");
	
	_rewardBoard = IMAGEMANAGER->findImage("RewardBoard");
	_rewardBoard->setX((WINSIZEX - 372) / 2);
	_rewardBoard->setY(WINSIZEY);

	_goldReward = IMAGEMANAGER->findImage("GoldReward");
	_goldReward->setY(WINSIZEY);
	_goldCoin = IMAGEMANAGER->findImage("Gold");
	_goldCoin->setY(WINSIZEY);
	_itemReward = IMAGEMANAGER->findImage("ItemReward");
	_itemReward->setY(WINSIZEY);

	// 플레이어 클래스
	if (_player == nullptr)
		_player = new player;

	_player->init();
	_player->hpBarInit(30, 50);
	_player->load();
	_player->getImage()->setFrameY(1);
	_player->getImage()->setX(100);
	_player->getImage()->setY(150);
	_playerCenterX = _player->getImage()->getX() + _player->getImage()->getFrameWidth() / 2;
	_evasion = false;
	_attack = false;

	// 테트리스 클래스
	if (_tetris == nullptr)
		_tetris = new tetris;

	_tetris->init();

	// 인벤토리 클래스
	if (_inventory == nullptr)
		_inventory = new inventory;

	_inventory->init(0, 0, WINSIZEX, WINSIZEY);

	// 에너미 클래스
	if (_enemy == nullptr)
		_enemy = new enemy;

	_enemy->init();

	// 옵션 클래스
	if (_option == nullptr)
		_option = new option;

	_option->init();

	// 스킬 클래스
	if (_skill == nullptr)
		_skill = new skill;

	_skill->init();

	// 입장 스테이지 정보 세팅
	int stage = -IMAGEMANAGER->findImage("ScreenOut")->getX();

	switch (stage)
	{
	case 1:
		_enemy->blueStoneInit(WINSIZEX / 2 + 70, 125, 0, 0);
		_enemyUnit = _enemy->getBlueStone();
		_enemy->hpBarInit(_enemyUnit, "StoneHp", "StoneHpBack", 310, 50);
		_enemy->actionBarInit(_enemyUnit, "StoneAction", "StoneActionBack", 360, 70);
		break;
	case 2:
		_enemy->darkBrownStoneInit(WINSIZEX / 2 + 70, 125, 0, 0);
		_enemyUnit = _enemy->getDarkBrownStone();
		_enemy->hpBarInit(_enemyUnit, "StoneHp", "StoneHpBack", 310, 50);
		_enemy->actionBarInit(_enemyUnit, "StoneAction", "StoneActionBack", 360, 70);
		break;
	case 3:
		_enemy->greenStoneInit(WINSIZEX / 2 + 70, 125, 0, 0);
		_enemyUnit = _enemy->getGreenStone();
		_enemy->hpBarInit(_enemyUnit, "StoneHp", "StoneHpBack", 310, 50);
		_enemy->actionBarInit(_enemyUnit, "StoneAction", "StoneActionBack", 360, 70);
		break;
	case 4:
		_enemy->orangeStoneInit(WINSIZEX / 2 + 70, 125, 0, 0);
		_enemyUnit = _enemy->getOrangeStone();
		_enemy->hpBarInit(_enemyUnit, "StoneHp", "StoneHpBack", 310, 50);
		_enemy->actionBarInit(_enemyUnit, "StoneAction", "StoneActionBack", 360, 70);
		break;
	case 5:
		_enemy->fullColorStoneInit(WINSIZEX / 2 + 70, 125, 0, 0);
		_enemyUnit = _enemy->getFullColorStone();
		_enemy->hpBarInit(_enemyUnit, "StoneHp", "StoneHpBack", 310, 50);
		_enemy->actionBarInit(_enemyUnit, "StoneAction", "StoneActionBack", 360, 70);
		break;
	}

	// 기존에 재생중인 사운드 종료
	SOUNDMANAGER->stop("Victory");
	SOUNDMANAGER->stop("Lose");

	// 사운드 Play 및 pause 여부
	if (!SOUNDMANAGER->isPauseSound("BattleBgm"))
		SOUNDMANAGER->play("BattleBgm");

	if (SOUNDMANAGER->isPauseSound("MainBgm"))
		SOUNDMANAGER->pause("BattleBgm");

	SOUNDMANAGER->stop("MainBgm");

	// 전투 종료시 변수값 초기화
	_battleEnd = false;

	// 피격 색상
	_red = CreateSolidBrush(RGB(255, 0, 0));

	// 전투 종료 후 팝업 관련 설정
	speed = 400;
	_replay.img = IMAGEMANAGER->findImage("Replay");
	_replay.img->setY(WINSIZEY);
	_replay.sound = true;
	_home.img = IMAGEMANAGER->findImage("Home");
	_home.img->setY(WINSIZEY);
	_home.sound = true;

	// 버프 기본 정보 초기화
	_atkBuff = 0;
	_atkDebuff = 0;

	for (int i = 0; i < 4; i++)
	{
		_buff[i].x = 35 + 40 * i;
		_buff[i].y = 80;
		_buff[i].alpha = 255;
		_buff[i].count = 0;
	}

	// 버프 이미지 등록
	_buff[0].img = IMAGEMANAGER->findImage("AtkBuff");
	_buff[1].img = IMAGEMANAGER->findImage("AtkDebuff");
	_buff[2].img = IMAGEMANAGER->findImage("Recovery");
	_buff[3].img = IMAGEMANAGER->findImage("Poison");

	// 화면 전환 초기화
	_screenChange = false;

	return S_OK;
}

void battle::release()
{
	// 플레이어 정보 저장후 릴리즈
	_player->save();
	_player->release();
	SAFE_DELETE(_player);

	_tetris->release();
	SAFE_DELETE(_tetris);

	_enemy->release();
	SAFE_DELETE(_enemy);

	// 인벤토리 정보 저장후 릴리즈
	_inventory->save("inventory.inv");
	_inventory->release();
	SAFE_DELETE(_inventory);

	_option->release();
	SAFE_DELETE(_option);

	_skill->release();
	SAFE_DELETE(_skill);

	_damage.clear();

	// 사운드 종료
	SOUNDMANAGER->stop("Victory");
	SOUNDMANAGER->stop("Lose");
}

void battle::update()
{
	// 화면 전환 완료
	if (_screenChange)
	{
		_screenOut->setY(_screenOut->getY() + 50);

		if (_screenOut->getY() >= -539)
		{
			this->release();
			SCENEMANAGER->loadScene("Main");

			return;
		}
	}
	// 화면 전환이 끝난 후
	else if (_screenOut->getY() > WINSIZEY)
	{
		// 버프가 중복되지 않도록하는 카운트와 전투 종료시 프레임 조절을 위한 카운트
		static int count = 0;
		static int poisonCount = 1;
		static int recoveryCount = 1;

		// ESC버튼으로 전투 종료
		if (KEYMANAGER->isOnceKeyDown(VK_ESCAPE))
		{
			_battleEnd = true;
			count = 100;
		}

		// 전투 종료시
		if (_battleEnd)
		{
			// 프레임 조절을 위해 카운트 설정
			if (count < 100)
			{
				count++;
			}
			else if (count >= 100)
			{
				// 팝업창 속도를 조절하여 등장이펙트 연출
				if (speed >= 1)
				{
					_rewardBoard->setY(_rewardBoard->getY() - speed);

					speed /= 2;
				}
				else if (speed > -10)
				{
					_rewardBoard->setY(_rewardBoard->getY() - speed);

					speed -= 3;
				}

				// 결과창 인터페이스
				_goldReward->setX(_rewardBoard->getX() + 50);
				_goldReward->setY(_rewardBoard->getY() + 70);

				_goldCoin->setX(_goldReward->getX());
				_goldCoin->setY(_goldReward->getY() + 50);

				_itemReward->setX(_goldReward->getX());
				_itemReward->setY(_goldReward->getY() + 150);

				_replay.img->setX(_rewardBoard->getX() + _rewardBoard->getWidth() - 120 - _replay.img->getFrameWidth() / 2);
				_replay.img->setY(_rewardBoard->getY() + _rewardBoard->getHeight() - 100);

				_home.img->setX(_rewardBoard->getX() + 120 - _home.img->getFrameWidth() / 2);
				_home.img->setY(_rewardBoard->getY() + _rewardBoard->getHeight() - 100);

				// 버튼 충돌 체크
				_option->crushButton(&_replay, 1, 0);
				_option->crushButton(&_home, 1, 0);

				// 좌클릭 했을때
				if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON))
				{
					// Home 버튼과 충돌중인 경우 
					if (PtInRect(&_home.img->boudingBoxWithFrame(), _ptMouse))
					{
						// 화면 전환을 위한 정보 초기화
						_screenOut->setX(0);
						_screenOut->setY(-_screenOut->getHeight());
						_screenChange = true;
						count = 0;
						poisonCount = 1;
						recoveryCount = 1;

						return;
					}

					// 다시시작 버튼과 충돌중인 경우
					if (PtInRect(&_replay.img->boudingBoxWithFrame(), _ptMouse))
					{
						// 정보 초기화하고 다시 시작
						this->release();
						this->init();
						count = 0;

						return;
					}
				}
			}
		}

		// 프레임 조절을 위한 조건문
		if (count % 4 == 0)
		{
			// 공격 버프가 적용된 상태인지 확인
			if (_tetris->getAtkBuff(10000))
			{
				// 버프가 적용되었을때 아이콘 alpha값 조절
				_buff[0].count++;

				if (_buff[0].count % 20 == 0)
				{
					_buff[0].alpha += 127 + _buff[0].alpha / 255;
				}

				// 버프 수치 입력
				_atkBuff = ATK_BUFF;
			}
			else
				_atkBuff = 0;

			// 공격 디버프가 적용된 상태인지 확인
			if (_tetris->getAtkDebuff(10000))
			{
				
				// 디버프가 적용되었을때 아이콘 alpha값 조절
				_buff[1].count++;

				if (_buff[1].count % 20 == 0)
				{
					_buff[1].alpha += 127 + _buff[1].alpha / 255;
				}

				// 디버프 수치 입력
				_atkDebuff = ATK_BUFF;
			}
			else
				_atkDebuff = 0;

			// 중독 상태인지 확인
			if (_tetris->getPoison(10000))
			{
				// 중독 아이콘 alpha값 조절
				_buff[2].count++;

				if (_buff[2].count % 20 == 0)
				{
					_buff[2].alpha += 127 + _buff[2].alpha / 255;
				}

				// 일정 시간마다 데미지를 주도록 count와 상수값을 곱하여 시간 체크
				if (!_tetris->getPoison(1000 * (poisonCount - 1)) && _tetris->getPoison(1000 * poisonCount))
				{
					poisonCount++;
					_player->setHp(_player->getHp() - 10);

					_damage.push_back({ 10, _playerCenterX, (int)_player->getImage()->getY() - 10, 0 });
				}
			}
			else
				poisonCount = 1;

			// 회복 상태인지 확인
			if (_tetris->getRecovery(10000))
			{
				// 회복 아이콘 alpha값 조절
				_buff[3].count++;

				if (_buff[3].count % 20 == 0)
				{
					_buff[3].alpha += 127 + _buff[3].alpha / 255;
				}

				// 일정 시간마다 회복하도록 count와 상수값을 곱하여 시간 체크
				if (!_tetris->getRecovery(1000 * (recoveryCount - 1)) && _tetris->getRecovery(1000 * recoveryCount))
				{
					_player->setHp(_player->getHp() + 10);

					recoveryCount++;
				}
			}
			else
				recoveryCount = 1;

			// 플레이어 클래스 및 에너미 피격  업데이트
			_player->update();
			_enemy->damagedUpdate(_enemyUnit);

			// 전투중일때
			if (!_battleEnd)
			{
				// 1번 스킬 슬롯 버튼 입력
				if (KEYMANAGER->isOnceKeyDown(_option->getSkillSlot1()))
				{
					// 쿨타임 확인하고 조건에 맞을시 해당 슬롯 스킬 효과 적용
					if (GetTickCount64() - _skill->getSkill1Ptr()->saveTimer >= _skill->getSkill1Ptr()->cooltime)
					{
						_skill->getSkill1Ptr()->saveTimer = _enemyUnit->atkTimer;
						_skill->getSkill1Ptr()->runTimer = _enemyUnit->atkTimer;
						_skill->getSkill1Ptr()->activate = true;
					}
				}

				// 2번 스킬 슬롯 버튼 입력
				if (KEYMANAGER->isOnceKeyDown(_option->getSkillSlot2()))
				{
					// 쿨타임 확인하고 조건에 맞을시 해당 슬롯 스킬 효과 적용
					if (GetTickCount64() - _skill->getSkill2Ptr()->saveTimer >= _skill->getSkill2Ptr()->cooltime)
					{
						_skill->getSkill2Ptr()->saveTimer = GetTickCount64();
						_skill->getSkill2Ptr()->runTimer = GetTickCount64();
						_skill->getSkill2Ptr()->activate = true;
					}
				}

				// 1번 아이템 슬롯 버튼 입력
				if (KEYMANAGER->isOnceKeyDown(_option->getItemSlot1()))
				{
					// 인벤토리에서 SmallPotion 사용 가능 여부 확인
					if (_inventory->usePotion("SmallPotion"))
					{
						// 사용시 체력 회복
						_player->setHp(_player->getHp() + _inventory->getSelectPtr()->value.value);
					}
				}

				// 2번 아이템 슬롯 버튼 입력
				if (KEYMANAGER->isOnceKeyDown(_option->getItemSlot2()))
				{
					// 인벤토리에서 LargePotion 사용 가능 여부 확인
					if (_inventory->usePotion("LargePotion"))
					{
						// 사용시 체력 회복
						_player->setHp(_player->getHp() + _inventory->getSelectPtr()->value.value);
					}
				}

				// 테트리스 클래스 업데이트
				_tetris->update();

				// 에너미 객체 업데이트
				if (_enemy->stoneUpdate(_enemyUnit))
				{
					// 플레이어가 회피 중이거나 공격중일때
					if (_evasion || _attack)
					{
						// Evasion 이미지를 출력하도록 준비된 정보를 저장
						_damage.push_back({ -1, _playerCenterX, (int)_player->getImage()->getY() - 10, 0 });
					}
					// 회피중이 아닐 경우
					else
					{
						// 에너미 공격력 - 플레이어 방어력
						int damage = _enemyUnit->atk - _player->getDef();

						// 데미지 최소 1로 고정
						if (damage < 1)
							damage = 1;

						// 플레이어 체력을 깎고 출력을 위한 정보 저장
						_player->setHp(_player->getHp() - damage);
						_player->setDamaged(true);

						_damage.push_back({ damage, _playerCenterX, (int)_player->getImage()->getY() - 10, 0 });
					}

					// 에너미의 공격 타이머 적용
					_skill->getSkill1Ptr()->runTimer = _enemyUnit->atkTimer;
				}

				// 블럭 내려놓았는지 확인
				if (GetTickCount64() - _tetris->getCombo().timer <= 100)
				{
					// 블럭이 지워졌다면 공격모션을 취한다.
					if (_tetris->getCombo().count)
					{
						_player->getImage()->setFrameX(0);
						_player->getImage()->setFrameY(5);
						_player->getImage()->setX(110);
						_player->setTimer(GetTickCount64());
						_attack = true;
					}
					// 블럭이 지워지지 않았다면 회피모션 프레임 설정
					else if (!_attack)
					{
						_player->getImage()->setFrameX(10);
						_player->getImage()->setFrameY(5);
						_player->getImage()->setX(90);
						_player->setTimer(GetTickCount64());
						_evasion = true;
					}
				}

				// 플레이어 체력이 0이하가 되었을 경우
				if (_player->getHp() <= 0)
				{
					// 에너미에게 준 피해 비율만큼 골드 획득
					_player->setGold(_player->getGold() + _enemyUnit->gold * (1.0f - (float)_enemyUnit->hp / _enemyUnit->maxHp));
					_battleEnd = true;
				}
				// 에너미 체력이 0이하가 되었을 경우
				else if (_enemyUnit->hp <= 0)
				{
					// 골드 및 드랍아이템 획득
					_player->setGold(_player->getGold() + _enemyUnit->gold);
					_inventory->setItem(_enemyUnit->itemName, _enemyUnit->itemType);
					_battleEnd = true;
				}
			}

			// 스킬 발동여부 확인 후 발동
			_skill->skillActivate(_skill->getSkill1Ptr(), _enemyUnit);
			_skill->skillActivate(_skill->getSkill2Ptr(), nullptr, _tetris->getSkillOnPtr());

			// 플레이어가 공격 상태일때
			if (_attack)
			{
				// 버그가 있어서 데미지 받는 중에는 공격을 받지 않도록 해두었던 부분인데
				// 스파게티 코드가 되어서 생긴 문제로 생각된다.
				if (!_enemyUnit->damaged)
				{
					_enemyUnit->hp -= _player->getAtk() + (_player->getAtk() * ((_tetris->getCombo().count - 1) * 10 + _tetris->getCombo().line * 50 + _atkBuff - _atkDebuff)) / 100;

					_damage.push_back({ _player->getAtk() + (_player->getAtk() * ((_tetris->getCombo().count - 1) * 10 + _tetris->getCombo().line * 50 + _atkBuff - _atkDebuff)) / 100, (int)_enemyUnit->img->getX() + _enemyUnit->img->getFrameWidth() / 2, (int)_enemyUnit->img->getY() - 10 });
					_enemyUnit->damaged = true;
				}

				// 공격 모션이 끝나면 false가 되도록 bool값 반환
				_attack = _player->playerAttack();
			}
			// 플레이어가 회피 상태일때
			else if (_evasion)
				// 회피 모션이 끝나면 false가 되도록 bool값 반환
				_evasion = _player->playerEvasion();
			// 플레이어 기본상태 동작
			else
				_player->playerIdle();
		}

		// 저장된 데미지 정보 alpha값과 y값 변경
		for (int i = 0; i < _damage.size(); i++)
		{
			_damage[i].y -= 1;

			if (_damage[i].alpha != 255)
				_damage[i].alpha += 3;
			else
			{
				_damage.erase(_damage.begin() + i);
				i--;
			}
		}
	}
	// 화면 전환 중인 경우
	else
		_screenOut->setY(_screenOut->getY() + 50);
}

void battle::render()
{
	// UI 렌더
	// 배경
	_bg->render(getMemDC());

	// 캐릭터창
	_room->render(getMemDC(), _room->getX(), _room->getY());
	_fieldGlass->alphaRender(getMemDC(), _fieldGlass->getX(), _fieldGlass->getY(), 150);
	_player->render();
	_player->hpBarRender();
	_enemy->stoneRender(_enemyUnit);

	// 스킬 및 아이템 슬롯
	for (int i = 0; i < 4; i++)
		_activeSlot->alphaRender(getMemDC(), _fieldGlass->getX() + 10 + 52 * i, _fieldGlass->getY() + _fieldGlass->getHeight() - 60, 150);

	// 스킬 아이콘 출력
	_skill->getSkill1Ptr()->img->render(getMemDC(), _fieldGlass->getX() + 10, _fieldGlass->getY() + _fieldGlass->getHeight() - 60);
	_skill->getSkill2Ptr()->img->render(getMemDC(), _fieldGlass->getX() + 62, _fieldGlass->getY() + _fieldGlass->getHeight() - 60);
	
	// 아이템 아이콘 출력
	IMAGEMANAGER->render("SmallPotion", getMemDC(), _fieldGlass->getX() + 114, _fieldGlass->getY() + _fieldGlass->getHeight() - 60);
	IMAGEMANAGER->render("LargePotion", getMemDC(), _fieldGlass->getX() + 166, _fieldGlass->getY() + _fieldGlass->getHeight() - 60);

	// 1번 스킬이 발동중일때
	if (_skill->getSkill1Ptr()->activate)
	{
		// 스킬 아이콘이 점점 검정색으로 덮어지도록 y의 길이를 조절하여 아이콘의 위치에 씌워준다.
		float y = 50 * (GetTickCount64() - _skill->getSkill1Ptr()->saveTimer) / _skill->getSkill1Ptr()->runTime;
		IMAGEMANAGER->alphaRender("SkillCoolTime", getMemDC(), _fieldGlass->getX() + 10, _fieldGlass->getY() + _fieldGlass->getHeight() - 60, 0, 0, 50, y, 150);
	}
	else
	{
		// 쿨타임 중인 경우 검정 이미지와 쿨타임 시간(초)를 출력한다.
		if (GetTickCount64() - _skill->getSkill1Ptr()->saveTimer < _skill->getSkill1Ptr()->cooltime)
		{
			IMAGEMANAGER->alphaRender("SkillCoolTime", getMemDC(), _fieldGlass->getX() + 10, _fieldGlass->getY() + _fieldGlass->getHeight() - 60, 150);
			printNumber((_skill->getSkill1Ptr()->cooltime - (GetTickCount64() - _skill->getSkill1Ptr()->saveTimer)) / 1000, _fieldGlass->getX() + 35, _fieldGlass->getY() + _fieldGlass->getHeight() - 50, 0);
		}
	}

	// 2번 스킬이 발동중일때
	if (_skill->getSkill2Ptr()->activate)
	{
		// 스킬 아이콘이 점점 검정색으로 덮어지도록 y의 길이를 조절하여 아이콘의 위치에 씌워준다.
		float y = 50 * (GetTickCount64() - _skill->getSkill2Ptr()->saveTimer) / _skill->getSkill2Ptr()->runTime;
		IMAGEMANAGER->alphaRender("SkillCoolTime", getMemDC(), _fieldGlass->getX() + 62, _fieldGlass->getY() + _fieldGlass->getHeight() - 60, 0, 0, 50, y, 150);
	}
	else
	{
		// 쿨타임 중인 경우 검정 이미지와 쿨타임 시간(초)를 출력한다.
		if (GetTickCount64() - _skill->getSkill2Ptr()->saveTimer < _skill->getSkill2Ptr()->cooltime)
		{
			IMAGEMANAGER->alphaRender("SkillCoolTime", getMemDC(), _fieldGlass->getX() + 62, _fieldGlass->getY() + _fieldGlass->getHeight() - 60, 150);
			printNumber((_skill->getSkill2Ptr()->cooltime - (GetTickCount64() - _skill->getSkill2Ptr()->saveTimer)) / 1000, _fieldGlass->getX() + 87, _fieldGlass->getY() + _fieldGlass->getHeight() - 50, 0);
		}
	}

	// 인벤토리에 소지중인 SmallPotion과 LargePotion의 갯수를 출력
	_inventory->printNumber(_inventory->getSmallPotionAmount(), _fieldGlass->getX() + 144, _fieldGlass->getY() + _fieldGlass->getHeight() - 30);
	_inventory->printNumber(_inventory->getLargePotionAmount(), _fieldGlass->getX() + 196, _fieldGlass->getY() + _fieldGlass->getHeight() - 30);

	// 공격 버프가 적용된 상태일때
	if (_tetris->getAtkBuff(10000))
	{
		// 체력바 밑에 스킬 아이콘이 표시되고 일정 시간마다 깜빡이도록 alpha값 설정
		_tetris->getAtkBuffImg()->alphaRender(getMemDC(), _buff[0].x, _buff[0].y, _buff[0].alpha);
		_inventory->printNumber(10 - _tetris->getAtkBuffTime() / 1000, _buff[0].x + 10, _buff[0].y + 10);
	}

	// 공격 디버프가 적용된 상태일때
	if (_tetris->getAtkDebuff(10000))
	{
		// 체력바 밑에 스킬 아이콘이 표시되고 일정 시간마다 깜빡이도록 alpha값 설정
		_tetris->getAtkDebuffImg()->alphaRender(getMemDC(), _buff[1].x, _buff[1].y, _buff[1].alpha);
		_inventory->printNumber(10 - _tetris->getAtkDebuffTime() / 1000, _buff[1].x + 10, _buff[1].y + 10);
	}

	// 중독 상태일때
	if (_tetris->getPoison(10000))
	{
		// 체력바 밑에 스킬 아이콘이 표시되고 일정 시간마다 깜빡이도록 alpha값 설정
		_tetris->getPoisonImg()->alphaRender(getMemDC(), _buff[2].x, _buff[2].y, _buff[2].alpha);
		_inventory->printNumber(10 - _tetris->getPoisonTime() / 1000, _buff[2].x + 10, _buff[2].y + 10);
	}

	// 회복 상태일때
	if (_tetris->getRecovery(10000))
	{
		// 체력바 밑에 스킬 아이콘이 표시되고 일정 시간마다 깜빡이도록 alpha값 설정
		_tetris->getRecoveryImg()->alphaRender(getMemDC(), _buff[3].x, _buff[3].y, _buff[3].alpha);
		_inventory->printNumber(10 - _tetris->getRecoveryTime() / 1000, _buff[3].x + 10, _buff[3].y + 10);
	}

	// 데미지 벡터에 저장된 정보를 가지고 데미지의 수치를 출력하거나 Evasion 문구 출력
	for (int i = 0; i < _damage.size(); i++)
	{
		if (_damage[i].damage == -1)
			IMAGEMANAGER->alphaRender("Evasion", getMemDC(), _damage[i].x - IMAGEMANAGER->findImage("Evasion")->getWidth() / 2, _damage[i].y, _damage[i].alpha);
		else if (_damage[i].damage != -2)
			printNumber(_damage[i].damage, _damage[i].x, _damage[i].y, _damage[i].alpha);
	}

	// 테트리스창
	_tetrisBoard->alphaRender(getMemDC(), _tetrisBoard->getX(), _tetrisBoard->getY(), 150);

	// 다음 블럭
	_nextBlock->alphaRender(getMemDC(), _nextBlock->getX(), _nextBlock->getY(), 150);
	_tetris->nextBlockRender(_nextBlock->getX() + 16, _nextBlock->getY() + _nextBlock->getHeight() - 48);

	// 홀드
	_holdBlock->alphaRender(getMemDC(), _holdBlock->getX(), _holdBlock->getY(), 150);
	_tetris->holdBlockRender(_holdBlock->getX() + 16, _holdBlock->getY() + _holdBlock->getHeight() - 48);

	// 테트리스 정보 그리기
	_tetris->render();

	// 콤보 띄웠을때 콤보 이미지 그리기
	_tetris->comboRender(WINSIZEX / 2, WINSIZEY / 2);

	// 전투 종료시
	if (_battleEnd)
	{
		// 리워드 목록 출력
		_rewardBoard->render(getMemDC(), _rewardBoard->getX(), _rewardBoard->getY());
		_goldReward->render(getMemDC(), _goldReward->getX(), _goldReward->getY());
		_goldCoin->render(getMemDC(), _goldCoin->getX(), _goldCoin->getY());
		_itemReward->render(getMemDC(), _itemReward->getX(), _itemReward->getY());

		// 에너미 체력이 0이하 일때
		if (_enemyUnit->hp <= 0)
		{
			// 획득 골드와 아이템 아이콘 출력
			printNumber(_enemyUnit->gold, _goldCoin->getX() + _goldCoin->getWidth() + 50, _goldCoin->getY(), 0);
			IMAGEMANAGER->render(_enemyUnit->itemName, getMemDC(), _itemReward->getX(), _itemReward->getY() + 50);

			// 승리 사운드 재생
			if (!SOUNDMANAGER->isPauseSound("BattleBgm"))
			{
				if (SOUNDMANAGER->isPlaySound("BattleBgm"))
				{
					SOUNDMANAGER->stop("BattleBgm");
					SOUNDMANAGER->play("Victory");
				}
			}
		}
		// 플레이어 체력이 0이하 일때
		else
		{
			// 데미지에 비례한 획득 골드량 출력
			printNumber(_enemyUnit->gold * (1.0f - (float)_enemyUnit->hp / _enemyUnit->maxHp), _goldCoin->getX() + _goldCoin->getWidth() + 50, _goldCoin->getY(), 0);

			// 패배 사운드 재생
			if (!SOUNDMANAGER->isPauseSound("BattleBgm"))
			{
				if (SOUNDMANAGER->isPlaySound("BattleBgm"))
				{
					SOUNDMANAGER->stop("BattleBgm");
					SOUNDMANAGER->play("Lose");
				}
			}
		}

		// 홈 및 리플레이 아이콘 출력
		_home.img->frameRender(getMemDC(), _home.img->getX(), _home.img->getY());
		_replay.img->frameRender(getMemDC(), _replay.img->getX(), _replay.img->getY());
	}

	// 화면 전환중일때 전환 이미지 출력
	if (_screenOut->getY() < WINSIZEY)
		_screenOut->render(getMemDC(), _screenOut->getX(), _screenOut->getY());
}

// 숫자 이미지 출력하는 함수
void battle::printNumber(int num, int x, int y, BYTE alpha)
{
	int digit = 1;
	int digitCount = 1;
	int _x = 0;

	while (num / digit >= 10)
	{
		digitCount++;
		digit *= 10;
	}

	_x = x - digitCount * IMAGEMANAGER->findImage("MidNumber")->getFrameWidth() / 2;

	for (int i = 0; ; i++, digit /= 10)
	{
		if (digit != 1)
			IMAGEMANAGER->alphaFrameRender("MidNumber", getMemDC(), _x + 25 * i, y, (num / digit) % 10, 0, alpha);
		else
		{
			IMAGEMANAGER->alphaFrameRender("MidNumber", getMemDC(), _x + 25 * i, y, num % 10, 0, alpha);
			break;
		}
	}
}
