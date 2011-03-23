/* vim:set et ts=4 sts=4:
 *
 * ibus-pinyin - The Chinese PinYin engine for IBus
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "PYConfig.h"

#include "PYTypes.h"
#include "PYBus.h"
#include "PYDoublePinyinTable.h"

namespace PY {

const gchar * const CONFIG_CORRECT_PINYIN            = "CorrectPinyin";
const gchar * const CONFIG_FUZZY_PINYIN              = "FuzzyPinyin";
const gchar * const CONFIG_ORIENTATION               = "LookupTableOrientation";
const gchar * const CONFIG_PAGE_SIZE                 = "LookupTablePageSize";
const gchar * const CONFIG_SHIFT_SELECT_CANDIDATE    = "ShiftSelectCandidate";
const gchar * const CONFIG_MINUS_EQUAL_PAGE          = "MinusEqualPage";
const gchar * const CONFIG_COMMA_PERIOD_PAGE         = "CommaPeriodPage";
const gchar * const CONFIG_AUTO_COMMIT               = "AutoCommit";
const gchar * const CONFIG_DOUBLE_PINYIN             = "DoublePinyin";
const gchar * const CONFIG_DOUBLE_PINYIN_SCHEMA      = "DoublePinyinSchema";
const gchar * const CONFIG_DOUBLE_PINYIN_SHOW_RAW    = "DoublePinyinShowRaw";
const gchar * const CONFIG_INIT_CHINESE              = "InitChinese";
const gchar * const CONFIG_INIT_FULL                 = "InitFull";
const gchar * const CONFIG_INIT_FULL_PUNCT           = "InitFullPunct";
const gchar * const CONFIG_INIT_SIMP_CHINESE         = "InitSimplifiedChinese";
const gchar * const CONFIG_SPECIAL_PHRASES           = "SpecialPhrases";
const gchar * const CONFIG_BOPOMOFO_KEYBOARD_MAPPING = "BopomofoKeyboardMapping";
const gchar * const CONFIG_SELECT_KEYS               = "SelectKeys";
const gchar * const CONFIG_GUIDE_KEY                 = "GuideKey";
const gchar * const CONFIG_AUXILIARY_SELECT_KEY_F    = "AuxiliarySelectKey_F";
const gchar * const CONFIG_AUXILIARY_SELECT_KEY_KP   = "AuxiliarySelectKey_KP";

boost::scoped_ptr<PinyinConfig> PinyinConfig::m_instance;
boost::scoped_ptr<BopomofoConfig> BopomofoConfig::m_instance;

Config::Config (Bus & bus, const std::string & name)
    : Object (ibus_bus_get_config (bus)),
      m_section ("engine/" + name)
{
    m_option = PINYIN_INCOMPLETE_PINYIN | PINYIN_CORRECT_ALL;
    m_option_mask = PINYIN_INCOMPLETE_PINYIN | PINYIN_CORRECT_ALL;

    m_orientation = IBUS_ORIENTATION_HORIZONTAL;
    m_page_size = 5;
    m_shift_select_candidate = FALSE;
    m_minus_equal_page = TRUE;
    m_comma_period_page = TRUE;
    m_auto_commit = FALSE;

    m_double_pinyin = FALSE;
    m_double_pinyin_schema = 0;
    m_double_pinyin_show_raw = FALSE;

    m_init_chinese = TRUE;
    m_init_full = FALSE;
    m_init_full_punct = TRUE;
    m_init_simp_chinese = TRUE;
    m_special_phrases = TRUE;

    g_signal_connect (get<IBusConfig> (),
                      "value-changed",
                      G_CALLBACK (valueChangedCallback),
                      this);
}

Config::~Config (void)
{
}

static const struct {
    const gchar * const name;
    guint option;
    bool defval;
} options [] = {
    { "IncompletePinyin",       PINYIN_INCOMPLETE_PINYIN,   TRUE },
    /* fuzzy pinyin */
    { "FuzzyPinyin_C_CH",       PINYIN_FUZZY_C_CH,          TRUE  },
    { "FuzzyPinyin_CH_C",       PINYIN_FUZZY_CH_C,          FALSE },
    { "FuzzyPinyin_Z_ZH",       PINYIN_FUZZY_Z_ZH,          TRUE  },
    { "FuzzyPinyin_ZH_Z",       PINYIN_FUZZY_ZH_Z,          FALSE },
    { "FuzzyPinyin_S_SH",       PINYIN_FUZZY_S_SH,          TRUE  },
    { "FuzzyPinyin_SH_S",       PINYIN_FUZZY_SH_S,          FALSE },
    { "FuzzyPinyin_L_N",        PINYIN_FUZZY_L_N,           TRUE  },
    { "FuzzyPinyin_N_L",        PINYIN_FUZZY_N_L,           FALSE },
    { "FuzzyPinyin_F_H",        PINYIN_FUZZY_F_H,           TRUE  },
    { "FuzzyPinyin_H_F",        PINYIN_FUZZY_H_F,           FALSE },
    { "FuzzyPinyin_L_R",        PINYIN_FUZZY_L_R,           FALSE },
    { "FuzzyPinyin_R_L",        PINYIN_FUZZY_R_L,           FALSE },
    { "FuzzyPinyin_K_G",        PINYIN_FUZZY_K_G,           TRUE  },
    { "FuzzyPinyin_G_K",        PINYIN_FUZZY_G_K,           TRUE  },
    { "FuzzyPinyin_AN_ANG",     PINYIN_FUZZY_AN_ANG,        TRUE  },
    { "FuzzyPinyin_ANG_AN",     PINYIN_FUZZY_ANG_AN,        TRUE  },
    { "FuzzyPinyin_EN_ENG",     PINYIN_FUZZY_EN_ENG,        TRUE  },
    { "FuzzyPinyin_ENG_EN",     PINYIN_FUZZY_ENG_EN,        TRUE  },
    { "FuzzyPinyin_IN_ING",     PINYIN_FUZZY_IN_ING,        TRUE  },
    { "FuzzyPinyin_ING_IN",     PINYIN_FUZZY_ING_IN,        TRUE  },
    { "FuzzyPinyin_IAN_IANG",   PINYIN_FUZZY_IAN_IANG,      TRUE  },
    { "FuzzyPinyin_IANG_IAN",   PINYIN_FUZZY_IANG_IAN,      TRUE  },
    { "FuzzyPinyin_UAN_UANG",   PINYIN_FUZZY_UAN_UANG,      TRUE  },
    { "FuzzyPinyin_UANG_UAN",   PINYIN_FUZZY_UANG_UAN,      TRUE  },
};

void
Config::readDefaultValues (void)
{
    /* others */
    m_orientation = read (CONFIG_ORIENTATION, 0);
    if (m_orientation != IBUS_ORIENTATION_VERTICAL &&
        m_orientation != IBUS_ORIENTATION_HORIZONTAL) {
        m_orientation = IBUS_ORIENTATION_HORIZONTAL;
        g_warn_if_reached ();
    }
    m_page_size = read (CONFIG_PAGE_SIZE, 5);
    if (m_page_size > 10) {
        m_page_size = 5;
        g_warn_if_reached ();
    }

    /* fuzzy pinyin */
    if (read (CONFIG_FUZZY_PINYIN, false))
        m_option_mask |= PINYIN_FUZZY_ALL;
    else
        m_option_mask &= ~PINYIN_FUZZY_ALL;

    /* read values */
    for (guint i = 0; i < G_N_ELEMENTS (options); i++) {
        if (read (options[i].name, options[i].defval))
            m_option |= options[i].option;
        else
            m_option &= ~options[i].option;
    }
}

inline bool
Config::read (const gchar * name,
              bool          defval)
{
    GValue value = {0};
    if (ibus_config_get_value (get<IBusConfig> (), m_section.c_str (), name, &value)) {
        if (G_VALUE_TYPE (&value) == G_TYPE_BOOLEAN)
            return g_value_get_boolean (&value);
    }

    // write default value to config
    g_value_init (&value, G_TYPE_BOOLEAN);
    g_value_set_boolean (&value, defval);
    ibus_config_set_value (get<IBusConfig> (), m_section.c_str (), name, &value);

    return defval;
}

inline gint
Config::read (const gchar * name,
              gint          defval)
{
    GValue value = {0};
    if (ibus_config_get_value (get<IBusConfig> (), m_section.c_str (), name, &value)) {
        if (G_VALUE_TYPE (&value) == G_TYPE_INT)
            return g_value_get_int (&value);
    }

    // write default value to config
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, defval);
    ibus_config_set_value (get<IBusConfig> (), m_section.c_str (), name, &value);

    return defval;
}

inline const gchar *
Config::read (const gchar * name,
              const gchar * defval)
{
    GValue value = {0};
    if (ibus_config_get_value (get<IBusConfig> (), m_section.c_str (), name, &value)) {
        if (G_VALUE_TYPE (&value) == G_TYPE_STRING)
            return g_value_get_string (&value);
    }

    // write default value to config
    g_value_init (&value, G_TYPE_STRING);
    g_value_set_static_string (&value, defval);
    ibus_config_set_value (get<IBusConfig> (), m_section.c_str (), name, &value);

    return defval;
}

static inline bool
normalizeGValue (const GValue *value, bool defval)
{
    if (value == NULL || G_VALUE_TYPE (value) != G_TYPE_BOOLEAN)
        return defval;
    return g_value_get_boolean (value);
}

static inline gint
normalizeGValue (const GValue *value, gint defval)
{
    if (value == NULL || G_VALUE_TYPE (value) != G_TYPE_INT)
        return defval;
    return g_value_get_int (value);
}

static inline const gchar *
normalizeGValue (const GValue *value, const gchar * defval)
{
    if (value == NULL || G_VALUE_TYPE (value) != G_TYPE_STRING)
        return defval;
    return g_value_get_string (value);
}

gboolean
Config::valueChanged (const std::string & section,
                      const std::string & name,
                      const GValue  *value)
{
    if (m_section != section)
        return FALSE;

    /* lookup table page size */
    if (CONFIG_ORIENTATION == name) {
        m_orientation = normalizeGValue (value, 0);
        if (m_orientation != IBUS_ORIENTATION_VERTICAL &&
            m_orientation != IBUS_ORIENTATION_HORIZONTAL) {
            m_orientation = IBUS_ORIENTATION_HORIZONTAL;
            g_warn_if_reached ();
        }
    }
    else if (CONFIG_PAGE_SIZE == name) {
        m_page_size = normalizeGValue (value, 5);
        if (m_page_size > 10) {
            m_page_size = 5;
            g_warn_if_reached ();
        }
    }
    /* fuzzy pinyin */
    else if (CONFIG_FUZZY_PINYIN == name) {
        if (normalizeGValue (value, TRUE))
            m_option_mask |= PINYIN_FUZZY_ALL;
        else
            m_option_mask &= ~PINYIN_FUZZY_ALL;
    }
    else {
        for (guint i = 0; i < G_N_ELEMENTS (options); i++) {
            if (G_LIKELY (options[i].name != name))
                continue;
            if (normalizeGValue (value, options[i].defval))
                m_option |= options[i].option;
            else
                m_option &= ~options[i].option;
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;

}

void
Config::valueChangedCallback (IBusConfig    *config,
                              const gchar   *section,
                              const gchar   *name,
                              const GValue  *value,
                              Config        *self)
{
    self->valueChanged (section, name, value);
}



static const struct {
    const gchar * const name;
    guint option;
    bool defval;
} pinyin_options [] = {
    /* correct */
    { "CorrectPinyin_GN_NG",    PINYIN_CORRECT_GN_TO_NG,    TRUE },
    { "CorrectPinyin_GN_NG",    PINYIN_CORRECT_GN_TO_NG,    TRUE },
    { "CorrectPinyin_MG_NG",    PINYIN_CORRECT_MG_TO_NG,    TRUE },
    { "CorrectPinyin_IOU_IU",   PINYIN_CORRECT_IOU_TO_IU,   TRUE },
    { "CorrectPinyin_UEI_UI",   PINYIN_CORRECT_UEI_TO_UI,   TRUE },
    { "CorrectPinyin_UEN_UN",   PINYIN_CORRECT_UEN_TO_UN,   TRUE },
    { "CorrectPinyin_UE_VE",    PINYIN_CORRECT_UE_TO_VE,    TRUE },
    { "CorrectPinyin_V_U",      PINYIN_CORRECT_V_TO_U,      TRUE },
    { "CorrectPinyin_VE_UE",    PINYIN_CORRECT_V_TO_U,      TRUE },
};

PinyinConfig::PinyinConfig (Bus & bus)
    : Config (bus, "Pinyin")
{
}

void
PinyinConfig::init (Bus & bus)
{
    if (m_instance == NULL) {
        m_instance.reset (new PinyinConfig (bus));
        m_instance->readDefaultValues ();
    }
}

void
PinyinConfig::readDefaultValues (void)
{
    Config::readDefaultValues ();
    /* double pinyin */
    m_double_pinyin = read (CONFIG_DOUBLE_PINYIN, false);
    m_double_pinyin_schema = read (CONFIG_DOUBLE_PINYIN_SCHEMA, 0);
    if (m_double_pinyin_schema > DOUBLE_PINYIN_LAST) {
        m_double_pinyin_schema = 0;
        g_warn_if_reached ();
    }
    m_double_pinyin_show_raw = read (CONFIG_DOUBLE_PINYIN_SHOW_RAW, false);

    /* init states */
    m_init_chinese = read (CONFIG_INIT_CHINESE, true);
    m_init_full = read (CONFIG_INIT_FULL, false);
    m_init_full_punct = read (CONFIG_INIT_FULL_PUNCT, true);
    m_init_simp_chinese = read (CONFIG_INIT_SIMP_CHINESE, true);

    m_special_phrases = read (CONFIG_SPECIAL_PHRASES, true);

    /* other */
    m_shift_select_candidate = read (CONFIG_SHIFT_SELECT_CANDIDATE, false);
    m_minus_equal_page = read (CONFIG_MINUS_EQUAL_PAGE, true);
    m_comma_period_page = read (CONFIG_COMMA_PERIOD_PAGE, true);
    m_auto_commit = read (CONFIG_AUTO_COMMIT, false);

    /* correct pinyin */
    if (read (CONFIG_CORRECT_PINYIN, true))
        m_option_mask |= PINYIN_CORRECT_ALL;
    else
        m_option_mask &= ~PINYIN_CORRECT_ALL;

    /* read values */
    for (guint i = 0; i < G_N_ELEMENTS (pinyin_options); i++) {
        if (read (pinyin_options[i].name, pinyin_options[i].defval))
            m_option |= pinyin_options[i].option;
        else
            m_option &= ~pinyin_options[i].option;
    }

}

gboolean
PinyinConfig::valueChanged (const std::string & section,
                      const std::string & name,
                      const GValue  *value)
{
    if (m_section != section)
        return FALSE;

    if (Config::valueChanged (section, name, value))
        return TRUE;

    /* double pinyin */
    if (CONFIG_DOUBLE_PINYIN == name)
        m_double_pinyin = normalizeGValue (value, false);
    else if (CONFIG_DOUBLE_PINYIN_SCHEMA == name) {
        m_double_pinyin_schema = normalizeGValue (value, 0);
        if (m_double_pinyin_schema > DOUBLE_PINYIN_LAST) {
            m_double_pinyin_schema = 0;
            g_warn_if_reached ();
        }
    }
    else if (CONFIG_DOUBLE_PINYIN_SHOW_RAW == name)
        m_double_pinyin_show_raw = normalizeGValue (value, false);
    /* init states */
    else if (CONFIG_INIT_CHINESE == name)
        m_init_chinese = normalizeGValue (value, true);
    else if (CONFIG_INIT_FULL == name)
        m_init_full = normalizeGValue (value, true);
    else if (CONFIG_INIT_FULL_PUNCT == name)
        m_init_full_punct = normalizeGValue (value, true);
    else if (CONFIG_INIT_SIMP_CHINESE == name)
        m_init_simp_chinese = normalizeGValue (value, true);
    else if (CONFIG_SPECIAL_PHRASES == name)
        m_special_phrases = normalizeGValue (value, true);
    /* others */
    else if (CONFIG_SHIFT_SELECT_CANDIDATE == name)
        m_shift_select_candidate = normalizeGValue (value, false);
    else if (CONFIG_MINUS_EQUAL_PAGE == name)
        m_minus_equal_page = normalizeGValue (value, true);
    else if (CONFIG_COMMA_PERIOD_PAGE == name)
        m_comma_period_page = normalizeGValue (value, true);
    else if (CONFIG_AUTO_COMMIT == name)
        m_auto_commit = normalizeGValue (value, false);
    /* correct pinyin */
    else if (CONFIG_CORRECT_PINYIN == name) {
        if (normalizeGValue (value, TRUE))
            m_option_mask |= PINYIN_CORRECT_ALL;
        else
            m_option_mask &= ~PINYIN_CORRECT_ALL;
    }
    else {
        for (guint i = 0; i < G_N_ELEMENTS (pinyin_options); i++) {
            if (G_LIKELY (pinyin_options[i].name != name))
                continue;
            if (normalizeGValue (value, pinyin_options[i].defval))
                m_option |= pinyin_options[i].option;
            else
                m_option &= ~pinyin_options[i].option;
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}


BopomofoConfig::BopomofoConfig (Bus & bus)
    : Config (bus, "Bopomofo")
{
}

void
BopomofoConfig::init (Bus & bus)
{
    if (m_instance == NULL) {
        m_instance.reset (new BopomofoConfig (bus));
        m_instance->readDefaultValues ();
    }
}

void
BopomofoConfig::readDefaultValues (void)
{
    Config::readDefaultValues ();

    /* init states */
    m_init_chinese = read (CONFIG_INIT_CHINESE, true);
    m_init_full = read (CONFIG_INIT_FULL, false);
    m_init_full_punct = read (CONFIG_INIT_FULL_PUNCT, true);
    m_init_simp_chinese = read (CONFIG_INIT_SIMP_CHINESE, false);

    m_special_phrases = read (CONFIG_SPECIAL_PHRASES, false);

    m_bopomofo_keyboard_mapping = read (CONFIG_BOPOMOFO_KEYBOARD_MAPPING, 0);

    m_select_keys = read (CONFIG_SELECT_KEYS, 0);
    if (m_select_keys >= 9) m_select_keys = 0;
    m_guide_key = read (CONFIG_GUIDE_KEY, true);
    m_auxiliary_select_key_f = read (CONFIG_AUXILIARY_SELECT_KEY_F, true);
    m_auxiliary_select_key_kp = read (CONFIG_AUXILIARY_SELECT_KEY_KP, true);
}

gboolean
BopomofoConfig::valueChanged (const std::string & section,
                              const std::string & name,
                              const GValue  *value)
{
    if (m_section != section)
        return FALSE;

    if (Config::valueChanged (section, name, value))
        return TRUE;

    /* init states */
    if (CONFIG_INIT_CHINESE == name)
        m_init_chinese = normalizeGValue (value, true);
    else if (CONFIG_INIT_FULL == name)
        m_init_full = normalizeGValue (value, true);
    else if (CONFIG_INIT_FULL_PUNCT == name)
        m_init_full_punct = normalizeGValue (value, true);
    else if (CONFIG_INIT_SIMP_CHINESE == name)
        m_init_simp_chinese = normalizeGValue (value, false);
    else if (CONFIG_SPECIAL_PHRASES == name)
        m_special_phrases = normalizeGValue (value, false);
    else if (CONFIG_BOPOMOFO_KEYBOARD_MAPPING == name)
        m_bopomofo_keyboard_mapping = normalizeGValue (value, 0);
    else if (CONFIG_SELECT_KEYS == name) {
        m_select_keys = normalizeGValue (value, 0);
        if (m_select_keys >= 9) m_select_keys = 0;
    }
    else if (CONFIG_GUIDE_KEY == name)
        m_guide_key = normalizeGValue (value, true);
    else if (CONFIG_AUXILIARY_SELECT_KEY_F == name)
        m_auxiliary_select_key_f = normalizeGValue (value, true);
    else if (CONFIG_AUXILIARY_SELECT_KEY_KP == name)
        m_auxiliary_select_key_kp = normalizeGValue (value, true);
    else
        return FALSE;
    return TRUE;

}
};
