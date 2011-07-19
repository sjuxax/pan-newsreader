const char * fallback_ui_file =
"<ui>\n"
"  <menubar name='main-window-menu'>\n"
"    <menu action='file-menu'>\n"
"      <menuitem action='import-tasks' />\n"
"      <menuitem action='cancel-last-task' />\n"
"      <menuitem action='work-online' />\n"
"      <separator />\n"
"      <menuitem action='show-task-window' />\n"
"      <menuitem action='show-log-dialog' />\n"
"      <separator />\n"
"      <menuitem action='quit' />\n"
"    </menu>\n"
"    <menu action='edit-menu'>\n"
"      <menuitem action='select-all-articles' />\n"
"      <menuitem action='add-subthreads-to-selection' />\n"
"      <menuitem action='add-threads-to-selection' />\n"
"      <menuitem action='add-similar-articles-to-selection' />\n"
"      <menuitem action='unselect-all-articles' />\n"
"      <menuitem action='select-article-body' />\n"
"      <separator />\n"
"      <menuitem action='show-servers-dialog' />\n"
"      <menuitem action='show-profiles-dialog' />\n"
"      <menuitem action='show-preferences-dialog' />\n"
"      <menuitem action='show-group-preferences-dialog' />\n"
"    </menu>\n"
"    <menu action='view-menu'>\n"
"      <menuitem action='show-toolbar' />\n"
"      <menu action='view-layout-menu'>\n"
"        <menuitem action='tabbed-layout' />\n"
"        <menuitem action='jump-to-group-tab' />\n"
"        <menuitem action='jump-to-header-tab' />\n"
"        <menuitem action='jump-to-body-tab' />\n"
"        <separator />\n"
"        <menuitem action='show-group-pane' />\n"
"        <menuitem action='show-header-pane' />\n"
"        <menuitem action='show-body-pane' />\n"
"      </menu>\n"
"      <menu action='view-group-pane-menu'>\n"
"        <menuitem action='shorten-group-names' />\n"
"      </menu>\n"
"      <menu action='view-header-pane-menu'>\n"
"        <menuitem action='thread-headers' />\n"
"        <separator />\n"
"        <menuitem action='enable-rules' />\n"
"        <separator />\n"
"        <menuitem action='show-matching-articles' />\n"
"        <menuitem action='show-matching-threads' />\n"
"        <menuitem action='show-matching-subthreads' />\n"
"        <separator />\n"
"        <menuitem action='match-only-unread-articles' />\n"
"        <menuitem action='match-only-cached-articles' />\n"
"        <menuitem action='match-only-binary-articles' />\n"
"        <menuitem action='match-only-my-articles' />\n"
"        <separator />\n"
"        <menuitem action='match-watched-articles' />\n"
"        <menuitem action='match-high-scoring-articles' />\n"
"        <menuitem action='match-medium-scoring-articles' />\n"
"        <menuitem action='match-normal-scoring-articles' />\n"
"        <menuitem action='match-low-scoring-articles' />\n"
"        <menuitem action='match-ignored-articles' />\n"
"      </menu>\n"
"      <menu action='view-body-pane-menu'>\n"
"        <menuitem action='wrap-article-body' />\n"
"        <menuitem action='mute-quoted-text' />\n"
"        <menuitem action='show-all-headers' />\n"
"        <menuitem action='monospace-font-enabled' />\n"
"        <menuitem action='show-smilies-as-graphics' />\n"
"        <menuitem action='show-text-markup'/>\n"
"        <menuitem action='highlight-urls'/>\n"
"        <menuitem action='rot13-selected-text' />\n"
"        <separator />\n"
"        <menuitem action='size-pictures-to-fit'/>\n"
"        <menuitem action='focus-on-image'/>\n"
"        <separator />\n"
"        <menuitem action='set-charset' />\n"
"      </menu>\n"
"    </menu>\n"
"    <menu action='go-menu'>\n"
"      <menuitem action='read-more' />\n"
"      <menuitem action='read-less' />\n"
"      <separator />\n"
"      <menuitem action='read-next-unread-group' />\n"
"      <menuitem action='read-next-group' />\n"
"      <separator />\n"
"      <menuitem action='read-next-unread-article' />\n"
"      <menuitem action='read-next-article' />\n"
"      <menuitem action='read-next-watched-article' />\n"
"      <menuitem action='read-previous-article' />\n"
"      <menuitem action='read-parent-article' />\n"
"      <separator />\n"
"      <menuitem action='read-next-unread-thread' />\n"
"      <menuitem action='read-next-thread' />\n"
"      <menuitem action='read-previous-thread' />\n"
"    </menu>\n"
//"    <menu action='actions-menu'>\n"
//"      <menuitem action='save-articles' />\n"
//"      <separator />\n"
"      <menu action='group-actions-menu'>\n"
"        <menuitem action='read-selected-group' />\n"
"        <menuitem action='mark-groups-read' />\n"
"        <separator />\n"
"        <menuitem action='download-headers' />\n"
"        <menuitem action='get-new-headers-in-selected-groups' />\n"
"        <menuitem action='get-new-headers-in-subscribed-groups'/>\n"
"        <separator />\n"
"        <menuitem action='show-group-preferences-dialog' />\n"
"        <separator />\n"
"        <menuitem action='subscribe' />\n"
"        <menuitem action='unsubscribe' />\n"
"        <separator />\n"
"        <menuitem action='refresh-group-list' />\n"
"        <separator />\n"
"        <menuitem action='delete-groups-articles' />\n"
"      </menu>\n"
"      <menu action='article-actions-menu'>\n"
"        <menuitem action='save-articles' />\n"
"        <menuitem action='save-articles-from-nzb' />\n"
"        <menuitem action='save-articles-to-nzb' />\n"
"        <separator />\n"
"        <menuitem action='read-selected-article' />\n"
"        <menuitem action='download-selected-article' />\n"
"        <menuitem action='show-selected-article-info' />\n"
"        <separator />\n"
"        <menuitem action='mark-article-read' />\n"
"        <menuitem action='mark-article-unread' />\n"
"        <separator />\n"
"        <menuitem action='add-article-score' />\n"
"        <menuitem action='watch-thread' />\n"
"        <menuitem action='ignore-thread' />\n"
"        <menuitem action='plonk' />\n"
"        <menuitem action='view-article-score' />\n"
"        <separator />\n"
"        <menuitem action='delete-article' />\n"
"      </menu>\n"
"      <menu action='posting-actions-menu'>\n"
"        <menuitem action='post' />\n"
"        <menuitem action='followup-to' />\n"
"        <menuitem action='reply-to' />\n"
"        <separator />\n"
"        <menuitem action='supersede-article' />\n"
"        <menuitem action='cancel-article' />\n"
"      </menu>\n"
"    <menu action='help-menu'>\n"
"      <menuitem action='pan-web-page' />\n"
"      <menuitem action='tip-jar' />\n"
"      <menuitem action='bug-report' />\n"
"      <separator />\n"
"      <menuitem action='about-pan' />\n"
"    </menu>\n"
"  </menubar>\n"
"\n"
"  <toolbar name='main-window-toolbar'>\n"
"    <toolitem action='download-headers' />\n"
"    <toolitem action='get-new-headers-in-subscribed-groups'/>\n"
"    <toolitem action='get-new-headers-in-selected-groups'/>\n"
"    <separator />\n"
"    <placeholder name='group-pane-filter'/>\n"
"    <separator />\n"
"    <toolitem action='post'/>\n"
"    <toolitem action='followup-to'/>\n"
"    <separator />\n"
"    <toolitem action='read-more' />\n"
"    <toolitem action='read-next-unread-article' />\n"
"    <toolitem action='read-next-unread-thread' />\n"
"    <separator />\n"
"    <toolitem action='save-articles' />\n"
"    <separator />\n"
"    <placeholder name='header-pane-filter' />\n"
"    <separator />\n"
"    <toolitem action='match-only-unread-articles' />\n"
"    <toolitem action='match-only-cached-articles' />\n"
"    <toolitem action='match-only-binary-articles' />\n"
"    <toolitem action='match-only-my-articles' />\n"
"    <toolitem action='match-only-watched-articles' />\n"
"  </toolbar>\n"
"\n"
"  <popup name='header-pane-popup'>\n"
"    <menuitem action='save-articles' />\n"
"    <menuitem action='save-articles-from-nzb' />\n"
"    <menuitem action='save-articles-to-nzb' />\n"
"    <separator />\n"
"    <menuitem action='read-selected-article' />\n"
"    <menuitem action='download-selected-article' />\n"
"    <menuitem action='show-selected-article-info' />\n"
"    <separator />\n"
"    <menuitem action='mark-article-read' />\n"
"    <menuitem action='mark-article-unread' />\n"
"    <separator />\n"
"    <menuitem action='add-article-score' />\n"
"    <menuitem action='watch-thread' />\n"
"    <menuitem action='ignore-thread' />\n"
"    <menuitem action='plonk' />\n"
"    <menuitem action='view-article-score' />\n"
"    <separator />\n"
"    <menuitem action='delete-article' />\n"
"  </popup>\n"
"\n"
"  <popup name='group-pane-popup'>\n"
"    <menuitem action='read-selected-group' />\n"
"    <menuitem action='mark-groups-read' />\n"
"    <separator />\n"
"    <menuitem action='download-headers' />\n"
"    <menuitem action='get-new-headers-in-selected-groups' />\n"
"    <menuitem action='get-new-headers-in-subscribed-groups'/>\n"
"    <separator />\n"
"    <menuitem action='post' />\n"
"    <separator />\n"
"    <menuitem action='show-group-preferences-dialog' />\n"
"    <separator />\n"
"    <menuitem action='subscribe' />\n"
"    <menuitem action='unsubscribe' />\n"
"    <separator />\n"
"    <menuitem action='refresh-group-list' />\n"
"    <separator />\n"
"    <menuitem action='delete-groups-articles' />\n"
"  </popup>\n"
"\n"
"</ui>\n";
