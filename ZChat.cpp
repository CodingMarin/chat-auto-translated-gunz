bool ZChat::Input(char* szMsg)
{
	if (0 == strlen(szMsg)) {
		return false;
	}

	int r = ZGetMyInfo()->GetRed();
	int g = ZGetMyInfo()->GetGreen();
	int b = ZGetMyInfo()->GetBlue();

	GunzState state = ZApplication::GetGameInterface()->GetState();

	std::string fromLang, toLang, text;
	if (ZTranslator::IsTranslationRequest(szMsg, fromLang, toLang, text))
	{
		int r = ZGetMyInfo()->GetRed();
		int g = ZGetMyInfo()->GetGreen();
		int b = ZGetMyInfo()->GetBlue();

		GunzState currentState = ZApplication::GetGameInterface()->GetState();
		int team = MMT_ALL;

		if (currentState == GUNZ_GAME)
		{
			ZCombatInterface* pCombat = ZGetGameInterface()->GetCombatInterface();
			if (pCombat->IsTeamChat())
				team = ZGetGame()->m_pMyCharacter->GetTeamID();
		}

		std::string copyFrom = fromLang;
		std::string copyTo = toLang;
		std::string copyText = text;

		std::thread([=]() {

			std::string translated;
			if (!ZTranslator::TranslateWithAPI(copyFrom, copyTo, copyText, translated))
			{
				ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), "[auto-translate] Error durante traducción");
				return;
			}

			char final[512];
			ZTranslator::FormatTranslatedMessage(translated, final, sizeof(final));

			switch (currentState)
			{
			case GUNZ_GAME:
				ZPostPeerChat(final, team, r, g, b);
				break;

			case GUNZ_LOBBY:
				ZPostChannelChat(
					ZGetGameClient()->GetPlayerUID(),
					ZGetGameClient()->GetChannelUID(),
					final, r, g, b
				);
				break;

			case GUNZ_STAGE:
				ZPostStageChat(
					ZGetGameClient()->GetPlayerUID(),
					ZGetGameClient()->GetStageUID(),
					final, r, g, b
				);
				break;
			}

			}).detach();

		return false;
	}


#ifdef _PUBLISH
	if ((timeGetTime() - m_nLastInputTime) < ZCHAT_CHAT_DELAY)
	{
		ZGetSoundEngine()->PlaySound("if_error");
		return false;
	}
#endif

	// Ŀ�ǵ� ���ɾ� ó�� //////////////////////
	bool bMsgIsCmd = false;
	if (szMsg[0] == '/')
	{
		if (strlen(szMsg) >= 2)
		{
			if (((szMsg[1] > 0) && (isspace(szMsg[1]))) == false)
			{
				ZChatCmdFlag nCurrFlag = CCF_NONE;

				switch (state)
				{
				case GUNZ_LOBBY: nCurrFlag = CCF_LOBBY; break;
				case GUNZ_STAGE: nCurrFlag = CCF_STAGE; break;
				case GUNZ_GAME:  nCurrFlag = CCF_GAME;  break;
				}

				int nCmdInputFlag = ZChatCmdManager::CIF_NORMAL;

				// ���������� �Ǻ�
				if ((ZGetMyInfo()->GetUGradeID() == MMUG_ADMIN) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_DEVELOPER) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_GAMEMASTER) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_EVENTTEAM))
				{
					nCmdInputFlag |= ZChatCmdManager::CIF_ADMIN;
				}
				// �׽������� �Ǻ� - test�����̰� launchdevelop����� ��쿡�� �׽���
				if ((ZIsLaunchDevelop()) && (!ZGetGameClient()->IsConnected()))
				{
					nCmdInputFlag |= ZChatCmdManager::CIF_TESTER;
				}

				bool bRepeatEnabled = m_CmdManager.IsRepeatEnabled(&szMsg[1]);
				if (!bRepeatEnabled)
				{
					if (!CheckRepeatInput(szMsg)) return false;
				}

				if (m_CmdManager.DoCommand(&szMsg[1], nCurrFlag, ZChatCmdManager::CmdInputFlag(nCmdInputFlag)))
				{
					return true;
				}
				else
				{
					if (szMsg[1] == '/')
						strcpy(&szMsg[0], &szMsg[1]);
					else
					{
						char buf[256];
						char* cmd = strtok(&szMsg[1], " ");
						sprintf(buf, "Unrecognized Command", cmd);
						ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), buf);
						return false;
					}
				}
			}
		}
	}


	if (!bMsgIsCmd)
	{
		if (!CheckRepeatInput(szMsg)) return false;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		ZChatOutput(ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM);
		return false;
	}

	// ����� //////////////////////////////////
	if (!CheckChatFilter(szMsg)) return false;

	// ���� Ŀ�ǵ� ���ɾ� //////////////////////
	bool bTeamChat = false;
	if (szMsg[0] == '!')
	{
		bTeamChat = true;
	}
	/*else if (szMsg[0] == '@')
	{
		ZPostChatRoomChat(&szMsg[1]);
		return true;
	}*/
	else if (szMsg[0] == '#')
	{
		if (ZGetGame() && ZGetGame()->m_pMyCharacter->IsAdminHide())
			ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ZMsg(MSG_SPECTATOR_CLAN));
		else
			ZPostClanMsg(ZGetGameClient()->GetPlayerUID(), &szMsg[1]);

		return true;
	}
	else if (szMsg[0] == '$')
	{
		ZPostChannelChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetChannelUID(), &szMsg[1], ZGetMyInfo()->GetRed(), ZGetMyInfo()->GetGreen(), ZGetMyInfo()->GetBlue());
		return true;
	}

	switch (state)
	{
	case GUNZ_GAME:
	{
		ZCombatInterface* pCombatInterface = ZGetGameInterface()->GetCombatInterface();
		int nTeam = MMT_ALL;
		if (pCombatInterface->IsTeamChat() || bTeamChat)
			nTeam = ZGetGame()->m_pMyCharacter->GetTeamID();

		if (ZGetGame() && ZGetGame()->m_pMyCharacter->IsAdminHide())
			ZPostPeerChat(szMsg, MMT_SPECTATOR, ZGetMyInfo()->GetRed(), ZGetMyInfo()->GetGreen(), ZGetMyInfo()->GetBlue());
		else
			ZPostPeerChat(szMsg, nTeam, ZGetMyInfo()->GetRed(), ZGetMyInfo()->GetGreen(), ZGetMyInfo()->GetBlue());
	}
	break;
	case GUNZ_LOBBY:
	{
		ZPostChannelChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetChannelUID(), szMsg, ZGetMyInfo()->GetRed(), ZGetMyInfo()->GetGreen(), ZGetMyInfo()->GetBlue());
	}
	break;
	case GUNZ_STAGE:
	{
		ZPostStageChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), szMsg, ZGetMyInfo()->GetRed(), ZGetMyInfo()->GetGreen(), ZGetMyInfo()->GetBlue());
	}
	break;
	}

	return true;
}