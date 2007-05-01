
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2006 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                          */
/*                                                                            */
/* Redistribution and use in source and binary forms, with or                 */
/* without modification, are permitted provided that the following            */
/* conditions are met:                                                        */
/*                                                                            */
/* Redistributions of source code must retain the above copyright             */
/* notice, this list of conditions and the following disclaimer.              */
/* Redistributions in binary form must reproduce the above copyright          */
/* notice, this list of conditions and the following disclaimer in            */
/* the documentation and/or other materials provided with the distribution.   */
/*                                                                            */
/* Neither the name of Rexx Language Association nor the names                */
/* of its contributors may be used to endorse or promote products             */
/* derived from this software without specific prior written permission.      */
/*                                                                            */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */
/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/******************************************************************************/
/* REXX  Support                                                              */
/*                                                                            */
/*                                                                            */
/* Error subcodes                                                             */
/*                                                                            */
/*   Note: The base codes are defined in document ANSI X3.274-1996            */
/*         ANS for Information Technology - Programming Language REXX         */
/*                                                                            */
/*        -- DO NOT CHANGE THIS FILE, ALL CHANGES WILL BE LOST! --            */
/*                  -- FILE WAS GENERATED BY genrxmsg --                      */
/******************************************************************************/

#ifndef RexxErrorCodes_INCLUDED
#define RexxErrorCodes_INCLUDED

/* error subcodes and message numbers                                         */
/*      message identifier ----------------------------------- message number */
   
#define Error_Program_unreadable                           3000
#define Error_Program_unreadable_name                      3001
#define Error_Program_unreadable_user_defined              3900
#define Error_Program_unreadable_notfound                  3901
#define Error_Program_unreadable_output_error              3902
#define Error_Program_unreadable_version                   3903
#define Error_Program_not_tokenized                        3904
#define Error_Program_interrupted                          4000
#define Error_Program_interrupted_condition                4001
#define Error_Program_interrupted_user_defined             4900
#define Error_System_resources                             5000
#define Error_System_resources_user_defined                5900
#define Error_Unmatched_quote                              6000
#define Error_Unmatched_quote_comment                      6001
#define Error_Unmatched_quote_single                       6002
#define Error_Unmatched_quote_double                       6003
#define Error_Unmatched_quote_user_defined                 6900
#define Error_When_expected                                7000
#define Error_When_expected_when                           7001
#define Error_When_expected_whenotherwise                  7002
#define Error_When_expected_nootherwise                    7003
#define Error_Unexpected_then                              8000
#define Error_Unexpected_then_then                         8001
#define Error_Unexpected_then_else                         8002
#define Error_Unexpected_when                              9000
#define Error_Unexpected_when_when                         9001
#define Error_Unexpected_when_otherwise                    9002
#define Error_Unexpected_end                               10000
#define Error_Unexpected_end_nodo                          10001
#define Error_Unexpected_end_control                       10002
#define Error_Unexpected_end_nocontrol                     10003
#define Error_Unexpected_end_then                          10005
#define Error_Unexpected_end_else                          10006
#define Error_Unexpected_end_select                        10004
#define Error_Unexpected_end_select_nolabel                10007
#define Error_Control_stack                                11000
#define Error_Control_stack_full                           11001
#define Error_Control_stack_user_defined                   11900
#define Error_Invalid_character                            13000
#define Error_Invalid_character_char                       13001
#define Error_Invalid_character_user_defined               13900
#define Error_Incomplete_do                                14000
#define Error_Incomplete_do_do                             14001
#define Error_Incomplete_do_select                         14002
#define Error_Incomplete_do_then                           14003
#define Error_Incomplete_do_else                           14004
#define Error_Incomplete_do_otherwise                      14901
#define Error_Invalid_hex                                  15000
#define Error_Invalid_hex_hexblank                         15001
#define Error_Invalid_hex_binblank                         15002
#define Error_Invalid_hex_invhex                           15003
#define Error_Invalid_hex_invbin                           15004
#define Error_Label_not_found                              16000
#define Error_Label_not_found_name                         16001
#define Error_Unexpected_procedure                         17000
#define Error_Unexpected_procedure_call                    17001
#define Error_Unexpected_procedure_interpret               17901
#define Error_Then_expected                                18000
#define Error_Then_expected_if                             18001
#define Error_Then_expected_when                           18002
#define Error_Symbol_or_string                             19000
#define Error_Symbol_or_string_address                     19001
#define Error_Symbol_or_string_call                        19002
#define Error_Symbol_or_string_name                        19003
#define Error_Symbol_or_string_signal                      19004
#define Error_Symbol_or_string_stream                      19912
#define Error_Symbol_or_string_trace                       19006
#define Error_Symbol_or_string_parse                       19007
#define Error_Symbol_or_string_user_defined                19900
#define Error_Symbol_or_string_class                       19901
#define Error_Symbol_or_string_method                      19902
#define Error_Symbol_or_string_routine                     19903
#define Error_Symbol_or_string_requires                    19904
#define Error_Symbol_or_string_external                    19905
#define Error_Symbol_or_string_metaclass                   19906
#define Error_Symbol_or_string_subclass                    19907
#define Error_Symbol_or_string_inherit                     19908
#define Error_Symbol_or_string_tilde                       19909
#define Error_Symbol_or_string_colon                       19911
#define Error_Symbol_or_string_mixinclass                  19913
#define Error_Symbol_expected                              20000
#define Error_Symbol_expected_user_defined                 20900
#define Error_Symbol_expected_drop                         20901
#define Error_Symbol_expected_expose                       20902
#define Error_Symbol_expected_parse                        20903
#define Error_Symbol_expected_var                          20904
#define Error_Symbol_expected_numeric                      20905
#define Error_Symbol_expected_varref                       20906
#define Error_Symbol_expected_leave                        20907
#define Error_Symbol_expected_iterate                      20908
#define Error_Symbol_expected_end                          20909
#define Error_Symbol_expected_on                           20911
#define Error_Symbol_expected_off                          20912
#define Error_Symbol_expected_use                          20913
#define Error_Symbol_expected_raise                        20914
#define Error_Symbol_expected_user                         20915
#define Error_Symbol_expected_directive                    20916
#define Error_Symbol_expected_colon                        20917
#define Error_Symbol_expected_LABEL                        20918
#define Error_Invalid_data                                 21000
#define Error_Invalid_data_user_defined                    21900
#define Error_Invalid_data_nop                             21901
#define Error_Invalid_data_select                          21902
#define Error_Invalid_data_name                            21903
#define Error_Invalid_data_condition                       21904
#define Error_Invalid_data_signal                          21905
#define Error_Invalid_data_trace                           21906
#define Error_Invalid_data_leave                           21907
#define Error_Invalid_data_iterate                         21908
#define Error_Invalid_data_end                             21909
#define Error_Invalid_data_form                            21911
#define Error_Invalid_data_guard_off                       21912
#define Error_Invalid_character_string                     22000
#define Error_Invalid_character_string_char                22001
#define Error_Invalid_character_string_DBCS                22901
#define Error_Invalid_character_string_user_defined        22900
#define Error_Invalid_data_string                          23000
#define Error_Invalid_data_string_char                     23001
#define Error_Invalid_data_string_user_defined             23900
#define Error_Invalid_trace                                24000
#define Error_Invalid_trace_trace                          24001
#define Error_Invalid_trace_debug                          24901
#define Error_Invalid_subkeyword                           25000
#define Error_Invalid_subkeyword_callon                    25001
#define Error_Invalid_subkeyword_calloff                   25002
#define Error_Invalid_subkeyword_signalon                  25003
#define Error_Invalid_subkeyword_signaloff                 25004
#define Error_Invalid_subkeyword_numeric                   25015
#define Error_Invalid_subkeyword_form                      25011
#define Error_Invalid_subkeyword_string_user_defined       25900
#define Error_Invalid_subkeyword_class                     25901
#define Error_Invalid_subkeyword_method                    25902
#define Error_Invalid_subkeyword_routine                   25903
#define Error_Invalid_subkeyword_requires                  25904
#define Error_Invalid_subkeyword_procedure                 25017
#define Error_Invalid_subkeyword_callonname                25914
#define Error_Invalid_subkeyword_signalonname              25915
#define Error_Invalid_subkeyword_parse                     25012
#define Error_Invalid_subkeyword_use                       25905
#define Error_Invalid_subkeyword_raise                     25906
#define Error_Invalid_subkeyword_raiseoption               25907
#define Error_Invalid_subkeyword_description               25908
#define Error_Invalid_subkeyword_additional                25909
#define Error_Invalid_subkeyword_result                    25911
#define Error_Invalid_subkeyword_guard_on                  25912
#define Error_Invalid_subkeyword_guard                     25913
#define Error_Invalid_subkeyword_forward_option            25916
#define Error_Invalid_subkeyword_to                        25917
#define Error_Invalid_subkeyword_arguments                 25918
#define Error_Invalid_subkeyword_continue                  25919
#define Error_Invalid_subkeyword_forward_class             25921
#define Error_Invalid_subkeyword_message                   25922
#define Error_Invalid_subkeyword_select                    25923
#define Error_Invalid_whole_number                         26000
#define Error_Invalid_whole_number_power                   26008
#define Error_Invalid_whole_number_repeat                  26002
#define Error_Invalid_whole_number_for                     26003
#define Error_Invalid_whole_number_parse                   26004
#define Error_Invalid_whole_number_digits                  26005
#define Error_Invalid_whole_number_fuzz                    26006
#define Error_Invalid_whole_number_trace                   26007
#define Error_Invalid_whole_number_intdiv                  26011
#define Error_Invalid_whole_number_rem                     26012
#define Error_Invalid_whole_number_method                  26901
#define Error_Invalid_whole_number_user_defined            26900
#define Error_Invalid_whole_number_compareto               26902
#define Error_Invalid_whole_number_compare                 26903
#define Error_Invalid_do                                   27000
#define Error_Invalid_do_whileuntil                        27001
#define Error_Invalid_do_forever                           27901
#define Error_Invalid_do_duplicate                         27902
#define Error_Invalid_leave                                28000
#define Error_Invalid_leave_leave                          28001
#define Error_Invalid_leave_iterate                        28002
#define Error_Invalid_leave_leavevar                       28003
#define Error_Invalid_leave_iteratevar                     28004
#define Error_Invalid_leave_iterate_name                   28005
#define Error_Environment_name                             29000
#define Error_Environment_name_name                        29001
#define Error_Name_too_long                                30000
#define Error_Name_too_long_name                           30001
#define Error_Name_too_long_string                         30002
#define Error_Name_too_long_user_defined                   30900
#define Error_Name_too_long_hex                            30901
#define Error_Name_too_long_bin                            30902
#define Error_Invalid_variable                             31000
#define Error_Invalid_variable_assign                      31001
#define Error_Invalid_variable_number                      31002
#define Error_Invalid_variable_period                      31003
#define Error_Invalid_variable_user_defined                31900
#define Error_Expression_result                            33000
#define Error_Expression_result_digits                     33001
#define Error_Expression_result_maxdigits                  33002
#define Error_Expression_user_defined                      33900
#define Error_Expression_result_address                    33901
#define Error_Expression_result_signal                     33902
#define Error_Expression_result_trace                      33903
#define Error_Expression_result_raise                      33904
#define Error_Logical_value                                34000
#define Error_Logical_value_if                             34001
#define Error_Logical_value_when                           34002
#define Error_Logical_value_while                          34003
#define Error_Logical_value_until                          34004
#define Error_Logical_value_logical                        34005
#define Error_Logical_value_logical_list                   34006
#define Error_Logical_value_user_defined                   34900
#define Error_Logical_value_method                         34901
#define Error_Logical_value_guard                          34902
#define Error_Logical_value_authorization                  34903
#define Error_Logical_value_property                       34904
#define Error_Invalid_expression                           35000
#define Error_Invalid_expression_general                   35001
#define Error_Invalid_expression_user_defined              35900
#define Error_Invalid_expression_prefix                    35901
#define Error_Invalid_expression_if                        35902
#define Error_Invalid_expression_when                      35903
#define Error_Invalid_expression_control                   35904
#define Error_Invalid_expression_by                        35905
#define Error_Invalid_expression_to                        35906
#define Error_Invalid_expression_for                       35907
#define Error_Invalid_expression_while                     35908
#define Error_Invalid_expression_until                     35909
#define Error_Invalid_expression_over                      35911
#define Error_Invalid_expression_interpret                 35912
#define Error_Invalid_expression_options                   35913
#define Error_Invalid_expression_address                   35914
#define Error_Invalid_expression_signal                    35915
#define Error_Invalid_expression_trace                     35916
#define Error_Invalid_expression_form                      35917
#define Error_Invalid_expression_assign                    35918
#define Error_Invalid_expression_operator                  35919
#define Error_Invalid_expression_guard                     35921
#define Error_Invalid_expression_raise_description         35922
#define Error_Invalid_expression_raise_additional          35923
#define Error_Invalid_expression_raise_list                35924
#define Error_Invalid_expression_forward_to                35925
#define Error_Invalid_expression_forward_arguments         35926
#define Error_Invalid_expression_forward_message           35927
#define Error_Invalid_expression_forward_class             35928
#define Error_Invalid_expression_logical_list              35929
#define Error_Invalid_expression_use_strict_default        35930
#define Error_Unmatched_parenthesis                        36000
#define Error_Unmatched_parenthesis_user_defined           36900
#define Error_Unmatched_parenthesis_paren                  36901
#define Error_Unmatched_parenthesis_square                 36902
#define Error_Unexpected_comma                             37000
#define Error_Unexpected_comma_comma                       37001
#define Error_Unexpected_comma_paren                       37002
#define Error_Unexpected_comma_user_defined                37900
#define Error_Unexpected_comma_bracket                     37901
#define Error_Invalid_template                             38000
#define Error_Invalid_template_trigger                     38001
#define Error_Invalid_template_position                    38002
#define Error_Invalid_template_with                        38003
#define Error_Invalid_template_user_defined                38900
#define Error_Invalid_template_missing                     38901
#define Error_Evaluation_stack_overflow                    39000
#define Error_Incorrect_call                               40000
#define Error_Incorrect_call_external                      40001
#define Error_Incorrect_call_result                        40901
#define Error_Incorrect_call_minarg                        40003
#define Error_Incorrect_call_maxarg                        40004
#define Error_Incorrect_call_noarg                         40005
#define Error_Incorrect_call_number                        40011
#define Error_Incorrect_call_whole                         40012
#define Error_Incorrect_call_nonnegative                   40013
#define Error_Incorrect_call_positive                      40014
#define Error_Incorrect_call_toobig                        40902
#define Error_Incorrect_call_range                         40903
#define Error_Incorrect_call_null                          40021
#define Error_Incorrect_call_option                        40022
#define Error_Incorrect_call_pad                           40023
#define Error_Incorrect_call_binary                        40024
#define Error_Incorrect_call_hex                           40025
#define Error_Incorrect_call_symbol                        40026
#define Error_Incorrect_call_list                          40904
#define Error_Incorrect_call_trace                         40905
#define Error_Incorrect_call_random                        40033
#define Error_Incorrect_call_sourceline                    40034
#define Error_Incorrect_call_x2d                           40035
#define Error_Incorrect_call_format_invalid                40019
#define Error_Incorrect_call_invalid_conversion            40029
#define Error_Incorrect_call_user_defined                  40900
#define Error_Incorrect_call_random_range                  40032
#define Error_Incorrect_call_stream_name                   40027
#define Error_Incorrect_call_array                         40912
#define Error_Incorrect_call_nostring                      40913
#define Error_Incorrect_call_selector                      40914
#define Error_Incorrect_call_parm_wrong_sep                40043
#define Error_Incorrect_call_format_incomp_sep             40044
#define Error_Incorrect_call_queue_no_char                 40915
#define Error_Incorrect_call_read_from_writeonly           40916
#define Error_Incorrect_call_write_to_readonly             40917
#define Error_Conversion                                   41000
#define Error_Conversion_operator                          41001
#define Error_Conversion_prefix                            41003
#define Error_Conversion_exponent                          41007
#define Error_Conversion_to                                41004
#define Error_Conversion_by                                41005
#define Error_Conversion_control                           41006
#define Error_Conversion_user_defined                      41900
#define Error_Conversion_raise                             41901
#define Error_Overflow                                     42000
#define Error_Overflow_overflow                            42001
#define Error_Overflow_underflow                           42002
#define Error_Overflow_zero                                42003
#define Error_Overflow_user_defined                        42900
#define Error_Overflow_expoverflow                         42901
#define Error_Overflow_expunderflow                        42902
#define Error_Overflow_power                               42903
#define Error_Routine_not_found                            43000
#define Error_Routine_not_found_name                       43001
#define Error_Routine_not_found_requires                   43901
#define Error_Routine_not_found_user_defined               43900
#define Error_Function_no_data                             44000
#define Error_Function_no_data_function                    44001
#define Error_Function_no_data_user_defined                44900
#define Error_No_data_on_return                            45000
#define Error_No_data_on_return_name                       45001
#define Error_Variable_reference                           46000
#define Error_Variable_reference_extra                     46001
#define Error_Variable_reference_user_defined              46900
#define Error_Variable_reference_missing                   46901
#define Error_Variable_reference_use                       46902
#define Error_Unexpected_label                             47000
#define Error_Unexpected_label_interpret                   47001
#define Error_System_service                               48000
#define Error_System_service_service                       48001
#define Error_System_service_user_defined                  48900
#define Error_Interpretation                               49000
#define Error_Interpretation_initialization                49001
#define Error_Interpretation_user_defined                  49900
#define Error_Variable_expected                            89000
#define Error_Variable_expected_USE                        89001
#define Error_Variable_expected_PARSE                      89002
#define Error_External_name_not_found                      90000
#define Error_External_name_not_found_user_defined         90900
#define Error_External_name_not_found_class                90997
#define Error_External_name_not_found_method               90998
#define Error_External_name_not_found_routine              90999
#define Error_No_result_object                             91000
#define Error_No_result_object_user_defined                91900
#define Error_No_result_object_message                     91999
#define Error_OLE_Error                                    92000
#define Error_OLE_Error_user_defined                       92900
#define Error_Unknown_OLE_Error                            92901
#define Error_Variant2Rexx                                 92902
#define Error_Rexx2Variant                                 92903
#define Error_Argument_Count_Mismatch                      92904
#define Error_Invalid_Variant                              92905
#define Error_OLE_Exception                                92906
#define Error_Unknown_OLE_Method                           92907
#define Error_Coercion_Failed_Overflow                     92908
#define Error_Coercion_Failed_Type_Mismatch                92909
#define Error_Parameter_Omitted                            92910
#define Error_No_OLE_instance                              92911
#define Error_Client_Disconnected_From_Server              92912
#define Error_Incorrect_method                             93000
#define Error_Incorrect_method_user_defined                93900
#define Error_Incorrect_method_minarg                      93901
#define Error_Incorrect_method_stream_type                 93958
#define Error_Incorrect_method_maxarg                      93902
#define Error_Incorrect_method_noarg                       93903
#define Error_Incorrect_method_number                      93904
#define Error_Incorrect_method_whole                       93905
#define Error_Incorrect_method_nonnegative                 93906
#define Error_Incorrect_method_positive                    93907
#define Error_Incorrect_method_toobig                      93908
#define Error_Incorrect_method_range                       93909
#define Error_Incorrect_method_null                        93911
#define Error_Incorrect_method_hex                         93912
#define Error_Incorrect_method_symbol                      93913
#define Error_Incorrect_method_list                        93914
#define Error_Incorrect_method_option                      93915
#define Error_Incorrect_method_string                      93916
#define Error_Incorrect_method_methodname                  93917
#define Error_Incorrect_method_index                       93918
#define Error_Incorrect_method_array                       93919
#define Error_Incorrect_method_binary                      93921
#define Error_Incorrect_method_pad                         93922
#define Error_Incorrect_method_length                      93923
#define Error_Incorrect_method_position                    93924
#define Error_Incorrect_method_minsub                      93925
#define Error_Incorrect_method_maxsub                      93926
#define Error_Incorrect_method_d2xd2c                      93927
#define Error_Incorrect_method_d2x                         93928
#define Error_Incorrect_method_d2c                         93929
#define Error_Incorrect_method_hexblank                    93931
#define Error_Incorrect_method_binblank                    93932
#define Error_Incorrect_method_invhex                      93933
#define Error_Incorrect_method_invbin                      93934
#define Error_Incorrect_method_x2dbig                      93935
#define Error_Incorrect_method_c2dbig                      93936
#define Error_Incorrect_method_supplier                    93937
#define Error_Incorrect_method_nostring                    93938
#define Error_Incorrect_method_noarray                     93939
#define Error_Incorrect_method_exponent_oversize           93941
#define Error_Incorrect_method_before_oversize             93942
#define Error_Incorrect_method_string_nonumber             93943
#define Error_Incorrect_method_nomessage                   93944
#define Error_Incorrect_method_message_noarg               93945
#define Error_Incorrect_method_message                     93946
#define Error_Incorrect_method_section                     93947
#define Error_Incorrect_method_noclass                     93948
#define Error_Incorrect_method_nomatch                     93949
#define Error_Incorrect_method_time                        93951
#define Error_Incorrect_method_no_method                   93961
#define Error_Incorrect_method_argType                     93953
#define Error_Incorrect_method_array_dimension             93954
#define Error_Incorrect_method_nostring_inarray            93952
#define Error_Incorrect_method_array_nostring              93956
#define Error_Incorrect_method_array_noclass               93957
#define Error_Incorrect_method_array_too_big               93959
#define Error_Incorrect_method_invbase64                   93962
#define Error_Unsupported_method                           93963
#define Error_Application_error                            93964
#define Error_Incorrect_method_abstract                    93965
#define Error_No_method                                    97000
#define Error_No_method_name                               97001
#define Error_No_method_user_defined                       97900
#define Error_Execution                                    98000
#define Error_Execution_user_defined                       98900
#define Error_Execution_nosomobj                           98901
#define Error_Execution_nodouble                           98902
#define Error_Execution_library                            98903
#define Error_Execution_terminate                          98904
#define Error_Execution_deadlock                           98905
#define Error_Execution_badobject                          98906
#define Error_Execution_wrongobject                        98907
#define Error_Execution_nometaclass                        98908
#define Error_Execution_noclass                            98909
#define Error_Execution_cyclic                             98911
#define Error_Execution_noSomclass                         98912
#define Error_Execution_noarray                            98913
#define Error_Execution_nostring                           98914
#define Error_Execution_message_reuse                      98915
#define Error_Execution_message_error                      98916
#define Error_Execution_raise_object                       98917
#define Error_Execution_propagate                          98918
#define Error_Execution_nomethod                           98919
#define Error_Execution_SOM_general                        98921
#define Error_Execution_SOM_noMethodDescriptor             98931
#define Error_Execution_SOM_unsupportedType                98932
#define Error_Execution_SOM_unsupportedReturnType          98933
#define Error_Execution_SOM_maxOutArgs                     98934
#define Error_Execution_reply                              98935
#define Error_Execution_reply_return                       98936
#define Error_Execution_reply_exit                         98937
#define Error_Execution_super                              98938
#define Error_Execution_syntax_additional                  98939
#define Error_Execution_error_condition                    98941
#define Error_Execution_mixinclass                         98942
#define Error_Execution_baseclass                          98943
#define Error_Execution_recursive_inherit                  98944
#define Error_Execution_uninherit                          98945
#define Error_Execution_forward_arguments                  98946
#define Error_Execution_forward                            98947
#define Error_Execution_authorization                      98948
#define Error_Execution_dsom_noserver                      98949
#define Error_Too_many_event_parms                         98961
#define Error_Creating_event                               98962
#define Error_Creating_event_direct_parm                   98963
#define Error_Accessing_aete                               98964
#define Error_Launching_app                                98965
#define Error_Invalid_event_additional_parm                98966
#define Error_Creating_event_additional_parm               98967
#define Error_Sending_event                                98968
#define Error_Handling_event_result                        98969
#define Error_Converting_event_result_to_rexx_obj          98971
#define Error_Invalid_event_direct_parm                    98972
#define Error_Invalid_object_specifier_type                98973
#define Error_Execution_no_concurrency                     98951
#define Error_Invalid_data_type_for_objspec                98974
#define Error_Execution_class_server_not_installed         98952
#define Error_Execution_sparse_array                       98975
#define Error_Translation                                  99000
#define Error_Translation_user_defined                     99900
#define Error_Translation_duplicate_class                  99901
#define Error_Translation_duplicate_method                 99902
#define Error_Translation_duplicate_routine                99903
#define Error_Translation_duplicate_requires               99904
#define Error_Translation_missing_class                    99905
#define Error_Translation_bad_metaclass                    99927
#define Error_Translation_expose                           99907
#define Error_Translation_expose_interpret                 99908
#define Error_Translation_guard                            99909
#define Error_Translation_guard_guard                      99911
#define Error_Translation_guard_interpret                  99912
#define Error_Translation_guard_expose                     99913
#define Error_Translation_directive_interpret              99914
#define Error_Translation_use_interpret                    99915
#define Error_Translation_bad_directive                    99916
#define Error_Translation_bad_external                     99917
#define Error_Translation_use_comma                        99918
#define Error_Translation_reply                            99919
#define Error_Translation_invalid_line                     99921
#define Error_Translation_requires                         99922
#define Error_Translation_reply_interpret                  99924
#define Error_Translation_forward_interpret                99923
#define Error_Translation_invalid_attribute                99925
#define Error_Translation_class_external_bad_parameters    99926
#define Error_Translation_class_external_bad_class_name    99928
#define Error_Translation_class_external_bad_class_server  99929
#define Error_Translation_use_strict_ellipsis              99930
#define Error_at_line                                      101000
#define Message_Translations_January                       101006
#define Message_Translations_February                      101007
#define Message_Translations_March                         101008
#define Message_Translations_April                         101009
#define Message_Translations_May                           101010
#define Message_Translations_June                          101011
#define Message_Translations_July                          101012
#define Message_Translations_August                        101013
#define Message_Translations_September                     101014
#define Message_Translations_October                       101015
#define Message_Translations_November                      101016
#define Message_Translations_December                      101017
#define Message_Translations_debug_prompt                  101005
#define Message_Translations_debug_error                   101004
#define Message_Translations_error                         101001
#define Message_Translations_running                       101002
#define Message_Translations_line                          101003
#define Error_RXSUBC_general                               999000
#define Error_RXSUBC_register                              999001
#define Error_RXSUBC_drop                                  999002
#define Error_RXSUBC_load                                  999003
#define Error_RXQUE_notinit                                999004
#define Error_RXQUE_size                                   999005
#define Error_RXQUE_nomem                                  999006
#define Error_RXQUE_name                                   999007
#define Error_RXQUE_access                                 999008
#define Error_RXQUE_exist                                  999010
#define Error_RXSUBC_query                                 999011
#define Error_REXXC_cmd_parm_incorrect                     999012
#define Error_REXXC_outDifferent                           999013
#define Error_REXXC_wrongNrArg                             999014
#define Error_REXXC_SynCheckInfo                           999015
#define Error_RXQUE_syntax                                 999016
#define Error_RXQUE_memfail                                999017


#endif

/* -------------------------------------------------------------------------- */
/* --            ==================================================        -- */
/* --            DO NOT CHANGE THIS FILE, ALL CHANGES WILL BE LOST!        -- */
/* --            ==================================================        -- */
/* -------------------------------------------------------------------------- */
   