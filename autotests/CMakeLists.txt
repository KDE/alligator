# SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_test(NAME alligator-self-test COMMAND alligator --self-test)
if (NOT BUILD_SHARED_LIBS)
    set_tests_properties(alligator-self-test PROPERTIES
        ENVIRONMENT "QT_QUICK_CONTROLS_STYLE=org.kde.breeze"
    )
endif()
