#if defined( TONE_MAPPING )

  fragColor.rgb = toneMapping( fragColor.rgb );

#endif
