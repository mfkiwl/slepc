#if !defined(__SLEPCVERSION_H)
#define __SLEPCVERSION_H

#define SLEPC_VERSION_RELEASE    0
#define SLEPC_VERSION_MAJOR      3
#define SLEPC_VERSION_MINOR      4
#define SLEPC_VERSION_SUBMINOR   0
#define SLEPC_VERSION_PATCH      0
#define SLEPC_RELEASE_DATE       "July 5, 2013"
#define SLEPC_VERSION_DATE       "unknown"

#if !defined (SLEPC_VERSION_GIT)
#define SLEPC_VERSION_GIT        "unknown"
#endif

#if !defined(SLEPC_VERSION_DATE_GIT)
#define SLEPC_VERSION_DATE_GIT   "unknown"
#endif

#define SLEPC_VERSION_(MAJOR,MINOR,SUBMINOR) \
 ((SLEPC_VERSION_MAJOR == (MAJOR)) &&       \
  (SLEPC_VERSION_MINOR == (MINOR)) &&       \
  (SLEPC_VERSION_SUBMINOR == (SUBMINOR)) && \
  (SLEPC_VERSION_RELEASE  == 1))

#define SLEPC_VERSION_LT(MAJOR,MINOR,SUBMINOR)          \
  (SLEPC_VERSION_RELEASE == 1 &&                        \
   (SLEPC_VERSION_MAJOR < (MAJOR) ||                    \
    (SLEPC_VERSION_MAJOR == (MAJOR) &&                  \
     (SLEPC_VERSION_MINOR < (MINOR) ||                  \
      (SLEPC_VERSION_MINOR == (MINOR) &&                \
       (SLEPC_VERSION_SUBMINOR < (SUBMINOR)))))))

#define SLEPC_VERSION_LE(MAJOR,MINOR,SUBMINOR) \
  (SLEPC_VERSION_LT(MAJOR,MINOR,SUBMINOR) || \
   SLEPC_VERSION_(MAJOR,MINOR,SUBMINOR))

#define SLEPC_VERSION_GT(MAJOR,MINOR,SUBMINOR) \
  (!SLEPC_VERSION_LE(MAJOR,MINOR,SUBMINOR))

#define SLEPC_VERSION_GE(MAJOR,MINOR,SUBMINOR) \
  (!SLEPC_VERSION_LT(MAJOR,MINOR,SUBMINOR))

#endif

