modded class MissionGameplay
{
	private ref WatermarkHandler m_watermarkHandler;
	ref Widget m_AdditionHudRootWidget = null;
	ref SyberiaAdditionalHud m_SyberiaAdditionalHud = null;
	ref array<int> m_pressedKeys;
	ref array<ref ToxicZoneView> m_toxicZonesView;
	float m_toxicZoneUpdateTimer;
	
	override void OnMissionStart()
	{
		SybLog("MissionGameplay OnMissionStart");
		super.OnMissionStart();
		SyberiaPPEffects.ResetAllEffects();
		m_pressedKeys = new array<int>;
		m_toxicZoneUpdateTimer = 0;
	}
	
	override void OnMissionFinish()
	{
		SybLog("MissionGameplay OnMissionStart");
		super.OnMissionFinish();
		
		delete m_AdditionHudRootWidget;
		delete m_SyberiaAdditionalHud;
		delete m_pressedKeys;
		SyberiaPPEffects.ResetAllEffects();
		
		if (m_toxicZonesView)
		{
			foreach (ref ToxicZoneView dtz : m_toxicZonesView)
			{
				delete dtz;
			}
			delete m_toxicZonesView;
		}
	}
	
	override void OnInit()
	{
		super.OnInit();
		
		m_watermarkHandler = new WatermarkHandler();
		SyberiaPPEffects.Init();
		
		if (!m_AdditionHudRootWidget)
		{
			m_AdditionHudRootWidget = GetGame().GetWorkspace().CreateWidgets("SyberiaScripts/layout/AdditionalHud.layout");
			m_AdditionHudRootWidget.Show(false);
			
			if ( !m_SyberiaAdditionalHud )
			{
				ref Widget actionBlocker = m_AdditionHudRootWidget.FindAnyWidget("ActionBlocker");
				m_AdditionHudRootWidget.RemoveChild(actionBlocker);
				
				ref MultilineTextWidget screenInfoWidget = MultilineTextWidget.Cast( m_AdditionHudRootWidget.FindAnyWidget("ScreenInfoWidget") );
				m_AdditionHudRootWidget.RemoveChild(screenInfoWidget);
				
				m_SyberiaAdditionalHud = new SyberiaAdditionalHud(m_HudRootWidget, actionBlocker, screenInfoWidget);
				m_SyberiaAdditionalHud.Init();		
			}
			
			if (m_Hud)
			{
				ref Widget notifications = m_AdditionHudRootWidget.FindAnyWidget("Notifications");				
				m_Hud.InitNotifierWidget(NTFKEY_SLEEPING, notifications, "Sleeping", 1);
				m_Hud.InitNotifierWidget(NTFKEY_MINDSTATE, notifications, "MindState", 2);
				
				ref Widget badgets = m_AdditionHudRootWidget.FindAnyWidget("Badgets");
				m_Hud.InitBadgetWidget(NTFKEY_BULLETHIT, badgets, "BulletHit");
				m_Hud.InitBadgetWidget(NTFKEY_KNIFEHIT, badgets, "KnifeHit");
				m_Hud.InitBadgetWidget(NTFKEY_HEMATOMA, badgets, "Hematoma");
				m_Hud.InitBadgetWidget(NTFKEY_VISCERADMG, badgets, "VisceraDamage");
				m_Hud.InitBadgetWidget(NTFKEY_CONCUSSION, badgets, "Concussion");
				m_Hud.InitBadgetWidget(NTFKEY_PAIN, badgets, "Pain");
				m_Hud.InitBadgetWidget(NTFKEY_PAINKILLER, badgets, "Painkiller");
				m_Hud.InitBadgetWidget(NTFKEY_ANTIBIOTIC, badgets, "Antibiotic");
				m_Hud.InitBadgetWidget(NTFKEY_STOMATCHHEAL, badgets, "Stomatchheal");
				m_Hud.InitBadgetWidget(NTFKEY_SEPSIS, badgets, "Sepsis");
				m_Hud.InitBadgetWidget(NTFKEY_ZVIRUS, badgets, "ZVirus");
				m_Hud.InitBadgetWidget(NTFKEY_BANDAGE1, badgets, "Bandage1");
				m_Hud.InitBadgetWidget(NTFKEY_BANDAGE2, badgets, "Bandage2");				
				m_Hud.InitBadgetWidget(NTFKEY_HEMOSTATIC, badgets, "Hemostatic");
				m_Hud.InitBadgetWidget(NTFKEY_HEMATOPOIESIS, badgets, "Hematopoiesis");
				m_Hud.InitBadgetWidget(NTFKEY_USESALVE, badgets, "UseSalve");
				m_Hud.InitBadgetWidget(NTFKEY_ADRENALIN, badgets, "Adrenalin");
				m_Hud.InitBadgetWidget(NTFKEY_OVERDOSED, badgets, "Overdosed");
				m_Hud.InitBadgetWidget(NTFKEY_INFLUENZA, badgets, "Influenza");
				m_Hud.InitBadgetWidget(NTFKEY_STOMATCHPOISONING, badgets, "Stomatchpoison");
				m_Hud.InitBadgetWidget(NTFKEY_RADIATIONSICKNESS, badgets, "RadiationSickness");
				m_Hud.InitBadgetWidget(NTFKEY_RADIOPROTECTION, badgets, "Radioprotection");
				m_Hud.InitBadgetWidget(NTFKEY_DISINFECTED, badgets, "Disinfected");
				m_Hud.InitBadgetWidget(NTFKEY_ANTIDEPRESANT, badgets, "Antidepresant");
			}
		}
		
		GetSyberiaRPC().RegisterHandler(SyberiaRPC.SYBRPC_SCREEN_MESSAGE, this, "OnScreenMessageRpc");
		GetSyberiaRPC().RegisterHandler(SyberiaRPC.SYBRPC_SYNC_TOXIC_ZONES, this, "OnSyncToxicZone");
	}
	
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
				
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if (player)
		{
			if (m_toxicZonesView)
			{
				m_toxicZoneUpdateTimer = m_toxicZoneUpdateTimer - timeslice;
				if (m_toxicZoneUpdateTimer <= 0.0)
				{
					m_toxicZoneUpdateTimer = 5.0;
					foreach (ref ToxicZoneView tzv : m_toxicZonesView)
					{
						tzv.Update(player);
					}
				}
			}
			
			SyberiaPPEffects.Update(timeslice);
			
			UIScriptedMenu menu = m_UIManager.GetMenu();
			
			if (m_SyberiaAdditionalHud && m_LifeState == EPlayerStates.ALIVE && !player.IsUnconscious() )
			{
				m_SyberiaAdditionalHud.Refresh(timeslice);	
			}
			
			if (m_Hud)
			{
				m_Hud.DisplayNotifier(NTFKEY_SLEEPING, player.GetSleepingTendency(), player.GetSleepingState());
				m_Hud.DisplayNotifier(NTFKEY_MINDSTATE, player.GetMindStateTendency(), player.GetMindState());
				
				m_Hud.DisplayBadge(NTFKEY_BULLETHIT, player.m_bulletHits);
				m_Hud.DisplayBadge(NTFKEY_KNIFEHIT, player.m_knifeHits);
				m_Hud.DisplayBadge(NTFKEY_HEMATOMA, player.m_hematomaHits);
				m_Hud.DisplayBadge(NTFKEY_VISCERADMG, player.m_visceraHit);
				m_Hud.DisplayBadge(NTFKEY_CONCUSSION, player.m_concussionHit);
				m_Hud.DisplayBadge(NTFKEY_PAIN, player.GetCurrentPainLevel());
				m_Hud.DisplayBadge(NTFKEY_PAINKILLER, player.m_painkillerEffect);
				m_Hud.DisplayBadge(NTFKEY_ANTIBIOTIC, player.m_antibioticsLevel);
				m_Hud.DisplayBadge(NTFKEY_STOMATCHHEAL, player.m_stomatchhealLevel);
				m_Hud.DisplayBadge(NTFKEY_SEPSIS, player.HasVisibleSepsis());
				m_Hud.DisplayBadge(NTFKEY_ZVIRUS, player.HasVisibleZVirus());
				m_Hud.DisplayBadge(NTFKEY_BANDAGE1, player.m_bulletBandage1 + player.m_knifeBandage1);
				m_Hud.DisplayBadge(NTFKEY_BANDAGE2, player.m_bulletBandage2 + player.m_knifeBandage2);
				m_Hud.DisplayBadge(NTFKEY_HEMOSTATIC, player.m_bloodHemostaticEffect);
				m_Hud.DisplayBadge(NTFKEY_HEMATOPOIESIS, player.m_hematopoiesisEffect);
				m_Hud.DisplayBadge(NTFKEY_USESALVE, player.m_salveEffect);
				m_Hud.DisplayBadge(NTFKEY_ADRENALIN, player.m_adrenalinEffect);
				m_Hud.DisplayBadge(NTFKEY_OVERDOSED, (int)Math.Floor(Math.Clamp(player.m_overdosedValue, 0, 3)));
				m_Hud.DisplayBadge(NTFKEY_INFLUENZA, player.m_influenzaLevel);
				m_Hud.DisplayBadge(NTFKEY_STOMATCHPOISONING, player.m_stomatchpoisonLevel);
				m_Hud.DisplayBadge(NTFKEY_RADIATIONSICKNESS, player.GetRadiationSicknessLevel());
				m_Hud.DisplayBadge(NTFKEY_RADIOPROTECTION, player.GetRadioprotectionLevel());
				m_Hud.DisplayBadge(NTFKEY_DISINFECTED, player.HasDisinfectedHands());
				m_Hud.DisplayBadge(NTFKEY_ANTIDEPRESANT, player.GetAntidepresantLevel());
			}
						
			OnUpdateAdvMedicineGUI(player, timeslice);
			OnUpdateMindstateGUI(player, timeslice);
			OnUpdatePveIntruderState(player, timeslice);
		}
		
		if (!player || !player.IsAlive())
		{
			if (m_HudRootWidget)
			{
				m_HudRootWidget.Show(false);
			}
		}
	}
	
	private void OnUpdateAdvMedicineGUI(PlayerBase player, float deltaTime)
	{		
		float overdosedEffect = Math.Clamp((player.m_overdosedValue - 1.0) * 0.1, 0, 0.3);
		SyberiaPPEffects.SetOverdosedEffect(overdosedEffect);
		
		float painEffect = Math.Clamp(player.GetCurrentPainLevel() * 0.1, 0, 0.3);
		SyberiaPPEffects.SetPainEffect(painEffect);
		
		SyberiaPPEffects.SetRadiationEffect(player.GetRadiationSicknessLevel());
		
		float concussionEffect = Math.Clamp(((int)player.m_concussionHit) * 0.1, 0, 0.1);
		SyberiaPPEffects.SetConcussionEffect(concussionEffect);
		
		SyberiaSleepingLevel sleepingLevel = player.GetSleepingProcessLevel();
		float sleepingValue = Math.Clamp((int)sleepingLevel, 0, 1);
		if (player.GetSleepingValue() == 0)
		{
			sleepingValue = 1;
		}
		SyberiaPPEffects.SetSleepingEffect(sleepingValue);
		
		SyberiaPPEffects.SetPsiEffect(player.m_zone != null && player.m_zone.m_psi > 0);
		
		if (sleepingLevel == SyberiaSleepingLevel.SYBSL_ENERGED)
		{
			m_SyberiaAdditionalHud.ShowScreenMessage("#syb_nosleep_energed", 2);
		}
		else if (sleepingLevel == SyberiaSleepingLevel.SYBSL_SICK)
		{
			m_SyberiaAdditionalHud.ShowScreenMessage("#syb_nosleep_sick", 2);
		}
		else if (sleepingLevel == SyberiaSleepingLevel.SYBSL_COLD)
		{
			m_SyberiaAdditionalHud.ShowScreenMessage("#syb_nosleep_cold", 2);
		}
		else if (sleepingLevel == SyberiaSleepingLevel.SYBSL_HOT)
		{
			m_SyberiaAdditionalHud.ShowScreenMessage("#syb_nosleep_hot", 2);
		}
	}
	
	private void OnUpdateMindstateGUI(PlayerBase player, float deltaTime)
	{
		if (player.GetMindStateValue() < GetSyberiaConfig().m_mindstateLevel4)
		{
			if (Math.RandomFloat01() < deltaTime * 0.05)
			{
				int action = Math.RandomInt(0, 8);
				if (action == 0)
				{
					Weapon_Base weapon;
					WeaponEventBase weapon_event = new WeaponEventTrigger;
					if ( Weapon_Base.CastTo(weapon, player.GetItemInHands()) )
					{
						weapon.ProcessWeaponEvent(weapon_event);
					}
				}
				else if (action == 1)
				{
					if (player.GetEmoteManager().CanPlayEmote(EmoteConstants.ID_EMOTE_SUICIDE))
					{
						player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SUICIDE);
					}
				}
				else
				{
					int emotesCount = player.GetEmoteManager().m_NameEmoteMap.Count();
					int emoteId = Math.RandomInt(0, emotesCount);
					int emoteKey = player.GetEmoteManager().m_NameEmoteMap.GetKey(emoteId);
					if (player.GetEmoteManager().CanPlayEmote(emoteKey))
					{
						player.GetEmoteManager().CreateEmoteCBFromMenu(emoteKey);
					}
				}				 
			}
		}
	}
	
	private void OnUpdatePveIntruderState(PlayerBase player, float deltaTime)
	{
		if (player.m_isPveIntruder && !player.m_isPveIntruderLast)
		{
			player.m_isPveIntruderLast = true;
			ShowScreenMessage("#syb_pve_intrude", 10);
		}
	}
	
	override int GetRespawnModeClient()
	{
		return GameConstants.RESPAWN_MODE_RANDOM;
	}
	
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		
		if (m_pressedKeys.Find(key) == -1)
		{
			m_pressedKeys.Insert(key);
		}
		
		PluginAdminTool pluginAdminTool;
		Class.CastTo(pluginAdminTool, GetPlugin(PluginAdminTool));
		if ( pluginAdminTool.m_adminPermissions && pluginAdminTool.m_freeCam )
		{
			pluginAdminTool.m_freeCam.HandleKey(key, true);
		}
	}
	
	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		
		int keyIndex = m_pressedKeys.Find(key);
		if (keyIndex != -1)
		{
			m_pressedKeys.Remove(keyIndex);
			OnKeyClicked(key);
		}
		
		PluginAdminTool pluginAdminTool;
		Class.CastTo(pluginAdminTool, GetPlugin(PluginAdminTool));
		if (pluginAdminTool)
		{
			if ( pluginAdminTool.m_adminPermissions && pluginAdminTool.m_freeCam )
			{
				pluginAdminTool.m_freeCam.HandleKey(key, false);
			}
		}
	}
	
	void OnKeyClicked(int key)
	{
		bool m_isAltPressed = m_pressedKeys.Find(KeyCode.KC_LMENU) != -1;
		
		PluginGearPDA pluginGearPDA;
		PluginSyberiaLogin pluginSyberiaLogin;
		if ( key == KeyCode.KC_ESCAPE )
		{	
			Class.CastTo(pluginGearPDA, GetPlugin(PluginGearPDA));
			if (pluginGearPDA && pluginGearPDA.IsOpen())
			{
				pluginGearPDA.Close();
			}
			
			Class.CastTo(pluginSyberiaLogin, GetPlugin(PluginSyberiaLogin));
			if (pluginSyberiaLogin)
			{
				pluginSyberiaLogin.CloseStethoscopeMenu();
			}
		}
		else if ( key == KeyCode.KC_RETURN )
		{
			Class.CastTo(pluginGearPDA, GetPlugin(PluginGearPDA));
			if (pluginGearPDA && pluginGearPDA.IsOpen())
			{
				pluginGearPDA.m_GearPDAMenu.m_externalSendEvent = true;
			}
		}
		
		PluginAdminTool pluginAdminTool;
		Class.CastTo(pluginAdminTool, GetPlugin(PluginAdminTool));
		if (pluginAdminTool)
		{
			if ( m_isAltPressed && key == KeyCode.KC_GRAVE )
			{			
				if (!pluginAdminTool.IsOpen())
				{
					pluginAdminTool.Open();
				}
			}
			else if ( m_isAltPressed && key == KeyCode.KC_T )
			{	
				if (pluginAdminTool.m_adminPermissions)
				{
					pluginAdminTool.TeleportToCursor();
				}
			}
			else if ( key == KeyCode.KC_ESCAPE )
			{	
				if (pluginAdminTool.IsOpen())
				{
					pluginAdminTool.Close();
				}
			}
		}
		
		DayZPlayerImplement playerClient = DayZPlayerImplement.Cast(GetGame().GetPlayer());
		if (key == KeyCode.KC_ESCAPE && playerClient && playerClient.m_skillsMenu && playerClient.m_skillsMenu.m_active)
		{
			playerClient.m_skillsMenu.m_active = false;
		}
	}

	protected void OnScreenMessageRpc(ref ParamsReadContext ctx, ref PlayerIdentity sender)
	{
		if (!m_SyberiaAdditionalHud) return;
		
		Param1<string> clientData;
       	if ( !ctx.Read( clientData ) ) return;	
		
		ShowScreenMessage(clientData.param1, 8);
	}
	
	override void ShowScreenMessage(string message, float time)
	{
		if (m_SyberiaAdditionalHud)
		{
			m_SyberiaAdditionalHud.ShowScreenMessage(message, time);
		}
	}
	
	override void Pause()
	{
		super.Pause();

		UIScriptedMenu ingameMenu = GetGame().GetUIManager().GetMenu();
		if (ingameMenu && ingameMenu.GetID() == MENU_INGAME)
		{
			ref Widget watermarkWidget = GetGame().GetWorkspace().CreateWidgets( "SyberiaScripts/layout/WatermarkInGame.layout" );		
			ref Widget watermarkBase = watermarkWidget.FindAnyWidget( "WatermarkBase" );
			ref Widget watermarkBtn = watermarkBase.FindAnyWidget( "WatermarkActionBtn" );
			TextWidget.Cast( watermarkBase.FindAnyWidget( "WatermarkTextWidget5" ) ).SetText(Syberia_Version);
			m_WidgetEventHandler.RegisterOnClick(watermarkBtn, m_watermarkHandler, "OnWatermarkClick");
			watermarkWidget.RemoveChild(watermarkBase);
			ingameMenu.GetLayoutRoot().AddChild(watermarkBase, true);
			delete watermarkWidget;
		}
	}
	
	void OnSyncToxicZone(ref ParamsReadContext ctx, ref PlayerIdentity sender)
	{
		Param1<ref array<ref ToxicZone>> clientData;
		if ( !ctx.Read( clientData ) ) return;
		
		ref array<ref ToxicZone> toxicZonesInfo = clientData.param1;
		m_toxicZonesView = new array<ref ToxicZoneView>;		
		if (toxicZonesInfo)
		{
			foreach (ref ToxicZone zone : toxicZonesInfo)
			{
				m_toxicZonesView.Insert(new ToxicZoneView(zone.m_position, zone.m_radius, zone.m_color));
			}
		}
	}
};