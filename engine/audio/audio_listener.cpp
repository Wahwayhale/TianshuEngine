#include "audio_listener.h"

namespace spark {

AudioListener::AudioListener() = default;
AudioListener::~AudioListener() = default;

void AudioListener::setPosition(Vec3 position) {
    m_position = position;
    // Update OpenAL listener position
}

void AudioListener::setVelocity(Vec3 velocity) {
    m_velocity = velocity;
    // Update OpenAL listener velocity
}

void AudioListener::setOrientation(Vec3 forward, Vec3 up) {
    m_forward = forward;
    m_up = up;
    // Update OpenAL listener orientation
}

} // namespace spark
