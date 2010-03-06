<?
function wikipedia_parse($text) {
  while(eregi("^(.*)\[\[([^\|\]*\|)?([^\[\|]*)\]\](.*)", $text, $m)) {
    $text=$m[1].$m[3].$m[4];
  }
  while(eregi("^(.*)'''([^']*)'''(.*)$", $text, $m)) {
    $text=$m[1].$m[2].$m[3];
  }
  return $text;
}

function wikipedia_url($object, $page, $lang) {
  if($page=="yes") {
    $page=$object->tags->get("name:$lang");
    if(!$page)
      $page=$object->tags->get("name");
    if(!$page)
      return;
  }

  if(preg_match("/^http.*\/wiki\/(.*)$/", $page, $m)) {
    $page=$m[1];
  }

  $page=strtr($page, array(" "=>"_"));

  return "http://$lang.wikipedia.org/wiki/$page";
}

function wikipedia_action_url($object, $page, $lang, $action) {
  if($page=="yes") {
    $page=$object->tags->get("name:$lang");
    if(!$page)
      $page=$object->tags->get("name");
    if(!$page)
      return;
  }

  if(preg_match("/^http.*\/wiki\/(.*)$/", $page, $m)) {
    $page=$m[1];
  }

  $page=strtr($page, array(" "=>"_"));

  return "http://$lang.wikipedia.org/w/index.php?title=$page&action=$action";
}

function wikipedia_parse_lang($object, $page, $lang, $data_lang) {
  if(!$url=wikipedia_action_url($object, $page, $lang, "raw"))
    return;

  ini_set("user_agent", "OpenStreetBrowser Wikipedia Parser");
  if(!(@$f=fopen($url, "r")))
    return;
  
  while($r=fgets($f)) {
    if(preg_match("/\[\[$data_lang:(.*)\]\]/", $r, $m)) {
      return $m[1];
    }
  }

  return;
}

function wikipedia_get_abstract($object, $page, $lang) {
  ini_set("user_agent", "OpenStreetBrowser Wikipedia Parser");
  if(@$f=fopen(wikipedia_action_url($object, $page, $lang, "raw"), "r")) {
    $text=""; unset($img);
    $enough=0;
    while(($r=fgets($f))&&(!$enough)) {
  //    if(!$img&&eregi("\[\[Bild:([^\|\]]*)[\|\]]", $r, $m)) {
      $r=chop($r);
      if(($r=="")||
	 (preg_match("/^<!--/", $r))
	) {
      }
      elseif(!$img&&eregi("\[\[.*:([^\|]*\.(png|jpg|gif))", $r, $m)) {
	$img=$m[1];
	$img="<img src='http://upload.wikimedia.org/wikipedia/commons/thumb/1/1c/$img/100px-$img' align='left' class='wikipedia_image'>\n";
      }
      elseif(!$img&&eregi("\|.*= *([^\|]*\.(png|jpg|gif))", $r, $m)) {
	$img=$m[1];
	$img="<img src='http://upload.wikimedia.org/wikipedia/commons/thumb/1/1c/$img/100px-$img' align='left' class='wikipedia_image'>\n";
      }
      elseif(!ereg("^[\|\}\{\[\!]", $r)) {
	$text.=wikipedia_parse($r);
	$enough=1;
      }
    }
    fclose($f);
  }

  return "<div class='wikipedia_abstract'>$img$text</div>";
}

function ext_wikipedia($object) {
  $ret="";
  global $data_lang;
  $page=0;

  if($page=$object->tags->get("wikipedia:$data_lang")) {
  }
  elseif($page=$object->tags->get("wikipedia")) {
    if(preg_match("/^http:\/\/([a-z]*)\..*/", $page, $m)) {
      $lang=$m[1];
      $page=$m[0];
      if($lang!=$data_lang) {
	if($new_page=wikipedia_parse_lang($object, $page, $lang, $data_lang)) {
	  $lang=$data_lang;
	  $page=$new_page;
	}
      }
    }
    elseif(preg_match("/^([a-z]*):(.*)/", $page, $m)) {
      $lang=$m[1];
      $page=$m[2];
      if($lang!=$data_lang) {
	if($new_page=wikipedia_parse_lang($object, $page, $lang, $data_lang)) {
	  $lang=$data_lang;
	  $page=$new_page;
	}
      }
    }
  }
  else {
    $list=$object->tags->get_available_languages("wikipedia");
    $lang=array_keys($list);
    $lang=$lang[0];
    $page=$list[$lang];

    if($new_page=wikipedia_parse_lang($object, $page, $lang, $data_lang)) {
      $lang=$data_lang;
      $page=$new_page;
    }
  }

  if(!$page)
    return;

  if(!($url=wikipedia_url($object, $page, $lang)))
    return;

  $text=wikipedia_get_abstract($object, $page, $lang);

  if($text) {
    $ret.="$text<a class='external' href='".urlencode($url)."'>".lang("read_more")."</a>";
  }

  return $ret;
}
