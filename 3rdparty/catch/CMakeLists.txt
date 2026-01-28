#**********************************
#  Created by boil on 2026/1/9.
#**********************************

if (RENDU_BUILD_TESTING)

    rendu_find_package(
            NAME Catch2
            GITHUB_REPOSITORY catchorg/Catch2
            GIT_TAG v3.11.0
    )

endif ()