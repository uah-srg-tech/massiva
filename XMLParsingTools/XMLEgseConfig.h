/**
 * \file	XMLEgseConfig.h
 * \brief	functions for parsing EGSE config file (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		06/06/2016 at 12:52:35
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLEgseConfig_H__
#define __XMLEgseConfig_H__

#include "../definitions.h"				/* gss_config, levelOut, portConfig */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function which parses GSS config
 * \param	filename			Name of GSS config file
 * \param[out]	config			Pointer to the GSS config struct
 * \param[out]	defaultOutput	Default GSS output to be configured
 * \return	0 if correct, a negative value if error occurred
 */
int EgseConfig(const char * filename, gss_config * pConfig,
        portConfig * ports, serialConfig * serials, gss_options * pOptions,
        portProtocol * protocols);
/**
 * \brief Function which parses the errors produced while parsing
 * \brief EGSE config file and copies an error message into a string
 * \param	status		The XML GSS config parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 */
int DisplayEgseConfigError (int status, portConfig * ports, char * msg,
	unsigned int maxMsgSize);

void UnconfigEgseConfig(gss_config * pConfig);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLEgseConfig_H__ */
