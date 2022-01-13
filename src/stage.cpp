/*!
  TinyDarius

  @file stage.cpp
  @brief Stage information
*/
#include "stage.hpp"
#include "typedef.hpp"

int Stage::_extra[] = { 'V', 'Z' }; // 26,27 (V',Z')
const Stage Stage::terminator = { -1, -1, -1, -1, 0, BGM(0)  };

const Stage Stage::table[Stage::MAX] =
{
    // ---- 0
#if 0
    {  0, 0,  1,  2, 2_fmin, BGM::CaptainNeo },     // A ->BC
#else
    {  0, 0, -1, -1, 2_fmin, BGM::CaptainNeo },     // A
#endif
    // ---- 1
#if 0
    {  1, 1,  3,  4, 3_fmin, BGM::InorganicBeat },  // B ->DE
    {  2, 1,  4,  5, 3_fmin, BGM::CosmicAirWay },   // C ->EF
#else
    {  1, 1, -1, -1, 3_fmin, BGM::InorganicBeat },  // B
    {  2, 1, -1, -1, 3_fmin, BGM::CosmicAirWay },   // C
#endif
    // ---- 2
    {  3, 2,  6,  7, 3_fmin, BGM::CosmicAirWay },   // D ->GH
    {  4, 2,  7,  8, 3_fmin, BGM::TheSea},          // E ->HI
    {  5, 2,  8,  9, 3_fmin ,BGM::ChaosMainTheme},  // F ->IJ
    // ---- 3
    {  6, 3, 10, 11, 4_fmin, BGM::ChaosMainTheme }, // G ->KL
    {  7, 3, 11, 12, 4_fmin, BGM::CaptainNeo },     // H ->LM
    {  8, 3, 12, 13, 4_fmin, BGM::InorganicBeat },  // I ->MN
    {  9, 3, 13, 14, 4_fmin, BGM::TheSea },         // J ->NO
    // ---- 4
    { 10, 4, 15, 16, 4_fmin, BGM::TheSea },         // K ->PQ
    { 11, 4, 16, 17, 4_fmin, BGM::InorganicBeat },  // L ->QR
    { 12, 4, 17, 18, 4_fmin, BGM::ChaosMainTheme }, // M ->RS
    { 13, 4, 18, 19, 4_fmin, BGM::CaptainNeo },     // M ->ST
    { 14, 4, 19, 20, 4_fmin, BGM::CosmicAirWay },   // O ->TU
    // ---- 5
    { 15, 5, 27, 21, 5_fmin, BGM::CaptainNeo },     // P ->Z'V
    { 16, 5, 21, 22, 5_fmin, BGM::CosmicAirWay },   // Q ->VW
    { 17, 5, 22, 23, 5_fmin, BGM::TheSea },         // R ->WX
    { 18, 5, 23, 24, 5_fmin, BGM::TheSea },         // S ->XY
    { 19, 5, 24, 25, 5_fmin, BGM::ChaosMainTheme }, // T ->YZ
    { 20, 5, 25, 26, 5_fmin, BGM::InorganicBeat },  // U ->ZV'
    // ---- 6
    { 21, 6, -1, -1, 6_fmin, BGM::ChaosMainTheme }, // V
    { 22, 6, -1, -1, 6_fmin, BGM::CaptainNeo },     // W
    { 23, 6, -1, -1, 6_fmin, BGM::CosmicAirWay},    // X
    { 24, 6, -1, -1, 6_fmin, BGM::InorganicBeat },  // Y
    { 25, 6, -1, -1, 6_fmin, BGM::TheSea },         // Z
    { 26, 6, -1, -1, 6_fmin, BGM::ChaosMainTheme }, // V'
    { 27, 6, -1, -1, 6_fmin, BGM::TheSea },         // Z'
};

/* 
   [Digression]

   If you want index of triangular numbers sequence to depth on the fly,

   std::int32_t index2depth(std::int32_t index)
   {
       return static_cast<std::int32_t>( -1 + std::sqrt(1 + 8.0f * i) * 0.5f);
   }

    0    - depth 0
   1 2   - depth 1
  3 4 5  - depth 2
 6 7 8 9 - depth 3
.........
*/
