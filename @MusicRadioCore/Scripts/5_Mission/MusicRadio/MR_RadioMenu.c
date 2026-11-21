// ============================================================================
//  MusicRadio - Core
//  MR_RadioMenu.c : the radio UI (overlay menu).
//  Layer: 5_Mission
//
//  Widget names come from gui/layouts/radio_ui.layout and radio_row.layout.
//  The menu is deliberately plain: vanilla fonts, no images, so it works on
//  every build and does not fight other UI mods.
//
//  Event routing
//  -------------
//  In DayZ a widget only delivers OnClick/OnChange to the handler that was set
//  ON THAT WIDGET - a parent's handler does not receive child events. So every
//  interactive widget is hooked to this menu explicitly in SetWidgetHandler().
// ============================================================================

// ---------------------------------------------------------------------------
//  Кто принимает клики
// ---------------------------------------------------------------------------
//  Widget.SetHandler() принимает ТОЛЬКО ScriptedWidgetEventHandler:
//      proto native void SetHandler(ScriptedWidgetEventHandler eventHandler);
//  А UIScriptedMenu происходит от UIMenuPanel (Managed) - обработчиком он не
//  является. Поэтому "m_Root.SetHandler(m_Events)" из UIScriptedMenu даёт на
//  КАЖДОЙ строке вызова:
//      Types 'MR_RadioMenu' and 'ScriptedWidgetEventHandler' are unrelated
//      Cannot convert 'MR_RadioMenu' to 'ScriptedWidgetEventHandler'
//  (лог 01:41, строки 112, 130, 307-311 - 20 наших ошибок).
//
//  Решение: меню остаётся UIScriptedMenu (иначе UIManager.ShowScriptedMenu его
//  не покажет и Init/Update не будут вызываться), а на виджеты вешается
//  маленький пересыльщик MR_MenuEvents. Так же устроена ваниль:
//  OptionsMenuGame/PlayerListScriptedWidget - это ScriptedWidgetEventHandler,
//  которым владеет меню-окно.
// ---------------------------------------------------------------------------

class MR_MenuEvents extends ScriptedWidgetEventHandler
{
	protected ref MR_RadioMenu m_Menu;

	void MR_MenuEvents(MR_RadioMenu menu)
	{
		m_Menu = menu;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnClick(w, x, y, button);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnChange(w, x, y, finished);
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnClick(w, x, y, button);
	}
}

class MR_RadioMenu extends UIScriptedMenu
{
	protected const string LAYOUT_MAIN = "MusicRadioCore/gui/layouts/radio_ui.layout";
	protected const string LAYOUT_ROW  = "MusicRadioCore/gui/layouts/radio_row.layout";

	// widgets
	protected Widget         m_Root;
	protected TextWidget     m_NowPlaying;
	protected TextWidget     m_TrackInfo;
	protected TextWidget     m_Status;
	protected TextWidget     m_Footer;
	protected TextWidget     m_VolumeLabel;
	protected TextWidget     m_RadiusLabel;
	protected SliderWidget   m_VolumeSlider;
	protected SliderWidget   m_RadiusSlider;
	protected ButtonWidget   m_PlayBtn;
	protected ButtonWidget   m_StopBtn;
	protected ButtonWidget   m_NextBtn;
	protected CheckBoxWidget m_Broadcast;
	protected EditBoxWidget  m_Search;
	protected WrapSpacerWidget m_List;

	// state
	protected int  m_Tab;                       // 0 music, 1 all tracks, 2 tapes/discs
	protected ref array<ref MR_Track> m_View;   // what the list shows right now
	protected ref map<Widget, ref MR_Track> m_RowTrack;
	protected int  m_Selected;
	protected ref MR_DeviceBase m_Host;
	protected MR_HostType m_HostType;
	protected int  m_HostIdLow;
	protected int  m_HostIdHigh;
	protected ref MR_MenuEvents m_Events;   // приёмник кликов, см. выше
	protected bool m_RowsBuilt;
	protected string m_AlbumId;                 // set when a tape/disc tab is shown

	void MR_RadioMenu()
	{
		m_Tab = 0;
		m_Selected = -1;
		m_View = new array<ref MR_Track>();
		m_RowTrack = new map<Widget, ref MR_Track>();
		m_HostType = MR_HostType.HOST_NONE;
		m_Events = new MR_MenuEvents(this);
	}

	// ----------------------------------------------------------------- setup
	override Widget Init()
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets(LAYOUT_MAIN);
		if (!m_Root)
		{
			MR.LogError("Cannot create UI from " + LAYOUT_MAIN);
			return null;
		}

		m_NowPlaying   = TextWidget.Cast(m_Root.FindAnyWidget("MR_NowPlaying"));
		m_TrackInfo    = TextWidget.Cast(m_Root.FindAnyWidget("MR_TrackInfo"));
		m_Status       = TextWidget.Cast(m_Root.FindAnyWidget("MR_StatusLabel"));
		m_Footer       = TextWidget.Cast(m_Root.FindAnyWidget("MR_Footer"));
		m_VolumeLabel  = TextWidget.Cast(m_Root.FindAnyWidget("MR_VolumeLabel"));
		m_RadiusLabel  = TextWidget.Cast(m_Root.FindAnyWidget("MR_RadiusLabel"));
		m_VolumeSlider = SliderWidget.Cast(m_Root.FindAnyWidget("MR_VolumeSlider"));
		m_RadiusSlider = SliderWidget.Cast(m_Root.FindAnyWidget("MR_RadiusSlider"));
		m_PlayBtn      = ButtonWidget.Cast(m_Root.FindAnyWidget("MR_PlayButton"));
		m_StopBtn      = ButtonWidget.Cast(m_Root.FindAnyWidget("MR_StopButton"));
		m_NextBtn      = ButtonWidget.Cast(m_Root.FindAnyWidget("MR_NextButton"));
		m_Broadcast    = CheckBoxWidget.Cast(m_Root.FindAnyWidget("MR_BroadcastCheck"));
		m_Search       = EditBoxWidget.Cast(m_Root.FindAnyWidget("MR_SearchBox"));
		m_List         = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("MR_TrackList"));

		MR_Settings s = MR_ClientState.Settings();

		if (m_VolumeSlider)
		{
			m_VolumeSlider.SetMinMax(0, s.m_MaxVolume);
			m_VolumeSlider.SetCurrent(s.m_DefaultVolume);
		}
		if (m_RadiusSlider)
		{
			m_RadiusSlider.SetMinMax(MR.RADIUS_MIN, s.m_MaxRadius);
			m_RadiusSlider.SetCurrent(s.m_DefaultRadius);
		}
		if (m_Broadcast)
			m_Broadcast.SetChecked(s.m_AllowBroadcast);

		SetWidgetHandler("MR_CloseButton");
		SetWidgetHandler("MR_TabMusic");
		SetWidgetHandler("MR_TabAll");
		SetWidgetHandler("MR_TabTapes");
		SetWidgetHandler("MR_PlayButton");
		SetWidgetHandler("MR_StopButton");
		SetWidgetHandler("MR_NextButton");
		SetWidgetHandler("MR_VolumeSlider");
		SetWidgetHandler("MR_RadiusSlider");
		SetWidgetHandler("MR_SearchBox");
		SetWidgetHandler("MR_BroadcastCheck");

		m_Root.SetHandler(m_Events);

		UpdateLabels();
		MR_ClientRPC.RequestLibrary();
		RefreshList();
		return m_Root;
	}

	// Attach this menu as the event handler of a named child widget.
	// NOTE: имя Hook не годится - в Object это protected-метод движка,
	// и компилятор отвечает 'Method Object is private'. Именно на этом
	// падала компиляция Mission в ревизии 20261165.
	void SetWidgetHandler(string name)
	{
		if (!m_Root)
			return;
		Widget w = m_Root.FindAnyWidget(name);
		if (w)
			w.SetHandler(m_Events);
	}

	override void OnShow()
	{
		super.OnShow();
		GetGame().GetInput().ChangeGameFocus(1);
		if (m_Search)
			SetFocus(m_Search);
	}

	override void OnHide()
	{
		// Tell the mission the menu is gone so F7 re-opens instead of closing.
		// ВАЖНО: этот OnHide - единственный в классе. Второй такой же давал
		//   Multiple declaration of function 'OnHide'   (лог 01:23, строка 560)
		// и Mission-модуль не собирался вообще.
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
			mission.MR_NotifyMenuClosed();

		super.OnHide();
		GetGame().GetInput().ChangeGameFocus(-1);
	}

	// Which host is this menu driving? Set by MissionGameplay before opening.
	void SetHost(MR_DeviceBase host, MR_HostType type, Object obj)
	{
		m_Host = host;
		m_HostType = type;
		if (obj)
		{
			MR.NetId(obj, m_HostIdLow, m_HostIdHigh);
		}
		if (host)
			MR_ClientState.SetOpenHost(host);
	}

	// ------------------------------------------------------------------ tabs
	void SelectTab(int tab)
	{
		m_Tab = tab;
		m_Selected = -1;
		m_RowsBuilt = false;
		RefreshList();
		UpdateLabels();
	}

	void ClearRows()
	{
		if (!m_List)
			return;

		m_RowTrack.Clear();

		Widget child = m_List.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			child.Unlink();
			child = next;
		}
	}

	void RefreshList()
	{
		if (!m_List)
			return;

		ClearRows();
		m_View.Clear();

		MR_Library lib = MR_ClientState.Library();
		string query = string.Empty;
		if (m_Search)
			query = m_Search.GetText();

		// Tab 2 is the "what is in my deck" view: one album, the physical one.
		m_AlbumId = string.Empty;
		if (m_Tab == 2)
		{
			m_AlbumId = AlbumInThisDeck();
			if (m_AlbumId == string.Empty)
			{
				CreatePlaceholder("No cassette or disc is loaded. Insert one into the player.");
				m_RowsBuilt = true;
				UpdateFooter(lib);
				return;
			}
			m_View = lib.ByAlbum(m_AlbumId);
		}
		else if (m_Tab == 0)
		{
			m_View = lib.Search(query, "music");
		}
		else
		{
			m_View = lib.Search(query, "");
		}

		int shown = 0;
		for (int i = 0; i < m_View.Count(); i++)
		{
			MR_Track t = m_View.Get(i);
			if (!t)
				continue;

			CreateRow(t, shown);
			shown = shown + 1;
		}

		if (shown == 0)
			CreatePlaceholder("Nothing here. Check the media pack or your search.");

		m_RowsBuilt = true;
		UpdateFooter(lib);
	}

	// Which album is physically loaded in the device this menu is driving?
	// Car: the deck docked on the dash. Handheld: the player-carried deck.
	string AlbumInThisDeck()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return string.Empty;

		if (m_HostType == MR_HostType.HOST_VEHICLE)
		{
			Object hostObj = MR_Registry.Find(m_HostType, m_HostIdLow, m_HostIdHigh);
			CarScript car = CarScript.Cast(hostObj);
			if (car)
				return car.MR_DockedAlbum();
			return string.Empty;
		}

		// HOST_ITEM = рация в руках: у неё теперь тоже есть вставленный
		// носитель, поэтому вкладка TAPES показывает альбом ИЗ РАЦИИ.
		if (m_HostType == MR_HostType.HOST_ITEM)
			return player.MR_HeldAlbum();

		return player.MR_CarriedAlbum();
	}

	void UpdateFooter(MR_Library lib)
	{
		if (!m_Footer)
			return;

		string text = "Library: " + lib.Count().ToString() + " tracks";
		if (m_AlbumId != string.Empty)
			text = "Album '" + m_AlbumId + "': " + m_View.Count().ToString() + " tracks (whole side, loops)";
		else if (m_Host && m_Host.MR_IsPlaying())
			text = text + "   |   next track starts automatically";

		m_Footer.SetText(text + "   |   F7 closes, F8 stops");
	}

	void CreateRow(MR_Track track, int index)
	{
		Widget row = GetGame().GetWorkspace().CreateWidgets(LAYOUT_ROW, m_List);
		if (!row)
			return;

		TextWidget idx    = TextWidget.Cast(row.FindAnyWidget("MR_RowIndex"));
		TextWidget title  = TextWidget.Cast(row.FindAnyWidget("MR_RowTitle"));
		TextWidget artist = TextWidget.Cast(row.FindAnyWidget("MR_RowArtist"));
		TextWidget dur    = TextWidget.Cast(row.FindAnyWidget("MR_RowDuration"));

		if (idx)    idx.SetText((index + 1).ToString());
		if (title)  title.SetText(track.m_Title);
		if (artist) artist.SetText(track.m_Artist);
		if (dur)    dur.SetText(track.DurationText());

		// Grey out entries this client cannot play (media pack missing / older).
		bool playable = (track.m_ResolveError == string.Empty && track.m_MediaKind != MR_MediaKind.MEDIA_NONE);
		if (!playable)
		{
			if (title)  title.SetColor(ARGB(255, 140, 90, 90));
			if (artist) artist.SetText("pack missing");
		}

		// Every clickable part of the row must point at this menu.
		row.SetHandler(m_Events);
		if (idx)    idx.SetHandler(m_Events);
		if (title)  title.SetHandler(m_Events);
		if (artist) artist.SetHandler(m_Events);
		if (dur)    dur.SetHandler(m_Events);

		m_RowTrack.Set(row, track);
	}

	void CreatePlaceholder(string text)
	{
		Widget row = GetGame().GetWorkspace().CreateWidgets(LAYOUT_ROW, m_List);
		if (!row)
			return;
		TextWidget title = TextWidget.Cast(row.FindAnyWidget("MR_RowTitle"));
		if (title)
		{
			title.SetText(text);
			title.SetColor(ARGB(255, 130, 130, 130));
		}
	}

	// ----------------------------------------------------------------- labels
	void UpdateLabels()
	{
		if (m_NowPlaying)
		{
			if (m_Host && m_Host.MR_IsPlaying())
				m_NowPlaying.SetText(m_Host.MR_GetTrackTitle());
			else
				m_NowPlaying.SetText("not playing");
		}

		if (m_Status)
			m_Status.SetText(MR_ClientState.GetMessage());

		if (m_VolumeSlider && m_VolumeLabel)
			m_VolumeLabel.SetText("VOLUME " + Math.Round(m_VolumeSlider.GetCurrent()).ToString() + "%");

		if (m_RadiusSlider && m_RadiusLabel)
			m_RadiusLabel.SetText("RANGE " + Math.Round(m_RadiusSlider.GetCurrent()).ToString() + " m");

		if (m_PlayBtn)
		{
			// тернарник - отдельным оператором, не аргументом (см. MR_Net.c)
			string label = "PLAY";
			if (m_Tab == 2)
				label = "PLAY TAPE";
			m_PlayBtn.SetText(label);
		}
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		if (m_Host && m_NowPlaying && m_Host.MR_IsPlaying())
		{
			string t = m_Host.MR_GetTrackTitle();
			if (m_Host.MR_GetDuration() > 0)
				t = t + "   " + MR_RadioMenu.TimeText(m_Host.MR_GetElapsedSeconds());
			m_NowPlaying.SetText(t);
		}
	}

	static string TimeText(int seconds)
	{
		int m = seconds / 60;
		int s = seconds % 60;
		return m.ToString() + ":" + MR.Pad2(s);
	}

	// ----------------------------------------------------------------- events
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!w)
			return false;

		string name = w.GetName();

		if (name == "MR_CloseButton")
		{
			Close();
			return true;
		}
		if (name == "MR_TabMusic")
		{
			SelectTab(0);
			return true;
		}
		if (name == "MR_TabAll")
		{
			SelectTab(1);
			return true;
		}
		if (name == "MR_TabTapes")
		{
			SelectTab(2);
			return true;
		}
		if (name == "MR_PlayButton")
		{
			DoPlay();
			return true;
		}
		if (name == "MR_StopButton")
		{
			DoStop();
			return true;
		}
		if (name == "MR_NextButton")
		{
			DoNext();
			return true;
		}

		// A click lands either on the row frame or on one of its texts: walk up
		// until the row is found.
		Widget row = w;
		int guard = 0;
		while (row && row.GetName() != "MR_Row" && row != m_Root && guard < 8)
		{
			row = row.GetParent();
			guard = guard + 1;
		}

		if (row && row.GetName() == "MR_Row")
		{
			SelectRow(row);
			if (button == MouseState.MIDDLE)
				DoPlay();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}

	void SelectRow(Widget row)
	{
		// m_RowTrack is authoritative; SetUserData is only a debugging aid.
		MR_Track t = m_RowTrack.Get(row);
		if (!t)
			return;

		m_Selected = m_View.Find(t);

		if (m_TrackInfo)
		{
			string year = "";
			if (t.m_Year > 0)
				year = " (" + t.m_Year.ToString() + ")";
			m_TrackInfo.SetText(t.DisplayName() + "\n" + t.m_Album + year + "\n" + t.DurationText() + "   " + t.m_Provider);
		}
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!w)
			return false;

		string name = w.GetName();

		if (name == "MR_VolumeSlider")
		{
			int vol = Math.Round(m_VolumeSlider.GetCurrent());
			if (m_Host && m_HostType != MR_HostType.HOST_NONE)
				MR_ClientRPC.RequestVolume(m_HostType, m_HostIdLow, m_HostIdHigh, vol);
			UpdateLabels();
			return true;
		}

		if (name == "MR_RadiusSlider")
		{
			UpdateLabels();
			return true;
		}

		if (name == "MR_SearchBox")
		{
			RefreshList();
			return true;
		}

		if (name == "MR_BroadcastCheck")
			return true;

		return super.OnChange(w, x, y, finished);
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			Close();
			return true;
		}
		return super.OnKeyDown(w, x, y, key);
	}

	// ---------------------------------------------------------------- actions
	protected MR_Track SelectedTrack()
	{
		if (m_Selected < 0 || m_Selected >= m_View.Count())
			return null;
		return m_View.Get(m_Selected);
	}

	void DoPlay()
	{
		// Tapes tab: "play" means "start the album that is in the deck".
		if (m_Tab == 2)
		{
			if (m_AlbumId == string.Empty)
			{
				MR_ClientState.SetMessage("No cassette or disc in the player");
				UpdateLabels();
				return;
			}
			if (!m_Host)
			{
				MR_ClientState.SetMessage("No player available here");
				UpdateLabels();
				return;
			}
			MR_ClientState.SetMessage("Starting album '" + m_AlbumId + "'");
			MR_ClientRPC.RequestPlayCarrier(m_Host, m_HostType, m_HostIdLow, m_HostIdHigh, m_AlbumId);
			UpdateLabels();
			return;
		}

		MR_Track t = SelectedTrack();
		if (!t)
		{
			MR_ClientState.SetMessage("Select a track first");
			UpdateLabels();
			return;
		}

		if (!m_Host)
		{
			MR_ClientState.SetMessage("No radio available here");
			UpdateLabels();
			return;
		}

		MR_ClientState.SetMessage("Requesting: " + t.DisplayName());
		MR_ClientRPC.RequestPlay(m_Host, m_HostType, m_HostIdLow, m_HostIdHigh, t.m_Id, t.m_MediaKind);
		UpdateLabels();
	}

	void DoStop()
	{
		if (!m_Host || m_HostType == MR_HostType.HOST_NONE)
			return;
		MR_ClientRPC.RequestStop(m_HostType, m_HostIdLow, m_HostIdHigh);
	}

	void DoNext()
	{
		if (!m_Host || m_HostType == MR_HostType.HOST_NONE)
			return;
		MR_ClientRPC.RequestNext(m_HostType, m_HostIdLow, m_HostIdHigh);
	}

}
